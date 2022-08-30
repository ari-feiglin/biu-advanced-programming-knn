#pragma once

#include <thread>
#include <mutex>
#include <functional>

namespace threading {
    /**
     * Class for representing a job for a thread to execute
     */
    class Job {
        std::function<void()> m_job;   
    
        public:
            template <typename... Params>
            Job(std::function<void(Params...)> job, Params... params) {
                this->m_job = [job, params...]() { (job)(params...); }
            }
    
            void operator()() { this->m_job(); }
    };
    
    class ThreadPool {
        bool should_terminate;
        std::mutex jobs_mutex;
        std::condition_variable mutex_condition;
        std::vector<std::thread> threads;
        std::queue<Job> jobs;

        void thread_loop();

        public:
            /**
             * Constructor for a ThreadPool.
             * Begins execution of ThreadPool as well.
             * @param num_threads       The number of threads to pool.
             *                          -1 to use the value of hardware_concurrency.
             */
            ThreadPool(unsigned int num_threads=-1);

            /**
             * Destructs the thread pool and joins all the threads.
             */
            ~ThreadPool();

            /**
             * Adds a job to the pool.
             * @param job           The job/function to run.
             * @param params        The parameters to pass to the job.
             */
            template <typename... Params>
            void add_job(std::function<void(Params...)>& job, Params... params);
    };
}

