#ifndef __MASTERSLAVE_H__
#define __MASTERSLAVE_H__
#include <mpi.h>

#include <cstdlib>
#include <cstring>
#include <functional>
#include <iterator>
#include <memory>
#include <vector>
#define MS_WORK_TAG (1 << 0)
#define MS_END_TAG (1 << 1)

namespace zey {
/**
 * @brief Master class in master-slave pattern, distributes a list of tasks to
 * other processes using MPI
 *
 * @tparam TASK_T A struct containing the arguments required to execute a task
 * @tparam RESULT_T struct containing the result of the task
 *
 * @param comm The group of MPI processes
 * @param tasks The list of tasks to execute
 * @param results The placeholders for the results
 * @param conclude A function to process the results
 * @param send_type The MPI_Datatype given to MPI_Send
 * @param send_size The amount of the datatype to be sent at once per process
 * @param received_type The MPI_Datatype given to MPI_Recv
 * @param received_size The amount of the datatype to be received
 */
template <class TASK_T, class RESULT_T>
void master(MPI_Comm comm, std::vector<TASK_T> tasks,
            std::vector<RESULT_T> &results,
            std::function<void *(const TASK_T &)> extract,
            std::function<void(void *, RESULT_T &)> insert,
            const MPI_Datatype send_type, const int send_size,
            const MPI_Datatype received_type, const int received_size,
            const int received_size_as_bytes) {
    int size{};
    int rank{};
    size_t task_send_index{0};
    size_t process_index{0};
    MPI_Status status;
    RESULT_T buff_holder{};
    auto buf_recv = std::make_unique<uint8_t[]>(received_size_as_bytes);
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    auto process_2_task_index = std::make_unique<size_t[]>(size);
    // send first batch of tasks
    for (process_index = 0; process_index < size; ++process_index) {
        // not send to itself
        if (process_index == rank) continue;
        // keep information about what task is executing each process
        process_2_task_index[process_index] = task_send_index;
        const TASK_T &buf_send = tasks[task_send_index];
        MPI_Send(extract(buf_send), send_size, send_type, process_index,
                 MS_WORK_TAG, comm);
        ++task_send_index;
    }
    // send the rest of the process
    for (task_send_index = size; task_send_index < tasks.size();
         ++task_send_index) {
        // store the result using lookup table
        MPI_Recv(buf_recv.get(), received_size, received_type, MPI_ANY_SOURCE,
                 MS_WORK_TAG, comm, &status);
        insert(buf_recv.get(), buff_holder);
        results[process_2_task_index[status.MPI_SOURCE]] = buff_holder;
        // send next task
        const TASK_T &buf_send = tasks[task_send_index];
        MPI_Send(extract(buf_send), send_size, send_type, status.MPI_SOURCE,
                 MS_WORK_TAG, comm);
        process_2_task_index[status.MPI_SOURCE] = task_send_index;
    }
    // recieve the last results (each slave still has a result)
    for (process_index = 0; process_index < size - 1; ++process_index) {
        MPI_Recv(buf_recv.get(), received_size, received_type, MPI_ANY_SOURCE,
                 MS_WORK_TAG, comm, &status);
        insert(buf_recv.get(), buff_holder);
        results[process_2_task_index[status.MPI_SOURCE]] = buff_holder;
    }
}

/**
 * @brief Master class in master-slave pattern, distributes a list of tasks to
 * other processes using MPI
 *
 * @tparam TASK_T A struct containing the arguments required to execute a task
 * @tparam RESULT_T struct containing the result of the task
 *
 * @param comm The group of MPI processes
 * @param tasks The list of tasks to execute
 * @param results The placeholders for the results
 * @param conclude A function to process the results
 * @param send_type The MPI_Datatype given to MPI_Send
 * @param send_size The amount of the datatype to be sent at once per process
 * @param received_type The MPI_Datatype given to MPI_Recv
 * @param received_size The amount of the datatype to be received
 */
template <class TASK_T, class RESULT_T, class OUTCOME_T, typename Iterator>
void master_variable_task(
    MPI_Comm comm, Iterator &tasks_begin, const Iterator &tasks_end,
    OUTCOME_T &results, std::function<void *(const TASK_T &)> extract,
    std::function<void(void *, RESULT_T &)> insert,
    std::function<void(RESULT_T &, OUTCOME_T &)> treatment,
    const MPI_Datatype send_type, const int send_size,
    const MPI_Datatype received_type, const int received_size,
    const int received_size_as_bytes) {
    int size{};
    int rank{};
    size_t task_send_index{0};
    size_t process_index{0};
    MPI_Status status;
    RESULT_T buff_holder{};
    auto buf_recv = std::make_unique<uint8_t[]>(received_size_as_bytes);
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    // send first batch of tasks
    for (process_index = 0; process_index < size;
         ++process_index, ++tasks_begin) {
        // not send to itself
        if (process_index == rank) continue;
        const TASK_T &buf_send = *tasks_begin;
        MPI_Send(extract(buf_send), send_size, send_type, process_index,
                 MS_WORK_TAG, comm);
        ++task_send_index;
    }
    // send the rest of the process
    for (; tasks_begin != tasks_end; ++tasks_begin) {
        // store the result using lookup table
        MPI_Recv(buf_recv.get(), received_size, received_type, MPI_ANY_SOURCE,
                 MS_WORK_TAG, comm, &status);
        insert(buf_recv.get(), buff_holder);
        treatment(buff_holder, results);
        // send next task
        const TASK_T &buf_send = *tasks_begin;
        MPI_Send(extract(buf_send), send_size, send_type, status.MPI_SOURCE,
                 MS_WORK_TAG, comm);
    }
    // receive the last results (each slave still has a result)
    for (process_index = 0; process_index < size - 1; ++process_index) {
        MPI_Recv(buf_recv.get(), received_size, received_type, MPI_ANY_SOURCE,
                 MS_WORK_TAG, comm, &status);
        insert(buf_recv.get(), buff_holder);
        treatment(buff_holder, results);
    }
}

void master_end_slaves(MPI_Comm comm) {
    int size{};
    int rank{};
    size_t process_index{0};
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    for (process_index = 0; process_index < size; ++process_index) {
        // not send to itself
        if (process_index == rank) continue;
        // keep information about what task is executing each process
        MPI_Send(&size, 0, MPI_BYTE, process_index, MS_END_TAG, comm);
    }
}
/**
 * @brief Slave in master-slave pattern, executes the tasks received from the
 * Master using MPI.
 *
 * @tparam TASK_T A struct containing the arguments required to execute a task
 * @tparam RESULT_T struct containing the result of the task
 *
 * @param comm The group of MPI processes
 * @param labor Function that defines what to do with the TASK_T structure
 * @param received_type The MPI_Datatype given to MPI_Recv
 * @param received_size The amount of the datatype to be received
 * @param send_type The MPI_Datatype given to MPI_Send
 * @param send_size The amount of the datatype to be sent at once per process
 */
template <class TASK_T, class RESULT_T>
void slave(MPI_Comm comm, std::function<RESULT_T(TASK_T &)> labor,
           std::function<void(void *, TASK_T &)> insert,
           std::function<void *(RESULT_T &)> extract,
           const MPI_Datatype received_type, const int received_size,
           const MPI_Datatype send_type, const int send_size,
           const int received_size_as_bytes) {
    MPI_Status status;
    TASK_T task;
    RESULT_T result;
    auto buff_insert = std::make_unique<uint8_t[]>(received_size_as_bytes);
    while (1) {
        MPI_Recv(buff_insert.get(), received_size, received_type,
                 MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &status);
        if (status.MPI_TAG != MS_WORK_TAG) {
            break;
        }
        insert(buff_insert.get(), task);
        result = labor(task);
        MPI_Send(extract(result), send_size, send_type, status.MPI_SOURCE,
                 MS_WORK_TAG, comm);
    }
}
}  // namespace zey
#endif  // __MASTERSLAVE_H__
