#pragma once

namespace threading {
    ThreadPool::ThreadPool(unsigned int num_threads) : should_terminate{false} {
        this->threads.resize(num_threads);

        /* Have all the threads run on the thread loop */
        for (unsigned int i = 0; i < num_threads; i++) {
            this->threads[i] = std::thread(&ThreadPool::thread_loop, this);
        }
    }

    void ThreadPool::end() {
        /* Block so the lock is released at the end. */
        {
            /* Lock the should_terminate so it can change it.
             * If this wasnt done, then a badly placed context switch could allow another job to be run. */
            std::unique_lock<std::mutex> lock{this->jobs_mutex};
            this->should_terminate = true;
        }

        /* Unblock all of the threads */
        this->mutex_condition.notify_all();

        /* Join the threads */
        for (std::thread& thread : this->threads) {
            thread.join();
        }
    }

    template <typename... Params>
    void ThreadPool::add_job(void (*job)(Params...), Params... params) {
        {
            /* Lock the jobs queue since it is shared readable/writable memory */
            std::unique_lock<std::mutex> lock{this->jobs_mutex};
            this->jobs.push(Job(job, params...));
        }

        /* Notify one of the threads waiting for a job that one is ready. */
        this->mutex_condition.notify_one();
    }

    void ThreadPool::thread_loop() {
        while (true) {
            Job job;

            {
                std::unique_lock<std::mutex> lock{this->jobs_mutex};
                this->mutex_condition.wait(lock, [this] {
                        return !this->jobs.empty() || this->should_terminate;
                    });

                if (this->should_terminate) return;

                job = this->jobs.front();
                this->jobs.pop();
            }   // Unlock the lock

            job();
        }
    }
}

