#pragma once

#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <vector>
#include <queue>

namespace threading {
    /**
     * Class for representing a job for a thread to execute
     */
    class Job {
        std::function<void()> m_job;   
    
        public:
            Job() { }

            template <typename... Params>
            Job(void (*job)(Params...), Params... params) {
                this->m_job = [job, params...]() { (job)(params...); };
            }

            Job(Job& other) : m_job{other.m_job} { }
            Job(Job&& other) : m_job{std::move(other.m_job)} { }

            Job& operator=(Job& other) { this->m_job = other.m_job; return *this; }
            Job& operator=(Job&& other) { this->m_job = std::move(other.m_job); return *this; }
    
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
             */
            ThreadPool(unsigned int num_threads);

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
            void add_job(void (*job)(Params...), Params... params);
    };
}

#include "thread-pool.tpp"

