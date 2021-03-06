/**
 * \file  job_manager.h
 * \brief Definition of JobManager class.
 *
 * FuD: FuDePAN Ubiqutous Distribution, a framework for work distribution.
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009, 2010, 2011 - Guillermo Biset & Mariano Bessone & Emanuel Bringas, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FuD providing class DistributableJob.
 *
 * System:         FuD
 * Homepage:       <http://fud.googlecode.com/>
 * Language:       C++
 *
 * @author     Guillermo Biset
 * @email      billybiset AT gmail.com
 *  
 * @author     Mariano Bessone
 * @email      marjobe AT gmail.com
 *
 * @author     Emanuel Bringas
 * @email      emab73 AT gmail.com
 *
 * FuD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FuD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FuD.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <list>
#include <boost/thread.hpp>
#include "fud/common/distributable_job.h"
#include "fud/common/job_unit.h"
#include "fud/server/clients_manager.h"
#include "fud/server/events.h"
#include "fud/server/synchronized_containers.h"

namespace fud
{
    /**
     * Interface to be implemented by the handler of events generated by the JobManager.
     * \sa Event
     */
    struct JobManagerEventHandler
    {
        //ClientsManager events
        /**
         * Performs actions to handle a free client.
         * Will likely try to assign a new JobUnit to a ClientProxy. Even though the event
         * corresponds to a particular ClientProxy, concurrent execution can not assure me
         * that this particular client will be used when trying to handle the event. So no
         * real reference to a ClientProxy is used.
         *
         * \sa ClientProxy
         */
        virtual void handle_free_client_event()                                          = 0;

        /**
         * Performs actions to handle the completion of a JobUnit.
         * It will need to locate the corresponding DistributableJob and tell it to handle
         * the message.
         *
         * @param id : The JobUnitID of the completed JobUnit.
         *
         * \sa DistributableJob
         * \sa JobUnit
         */
        virtual void handle_job_unit_completed_event(JobUnitID id)     = 0;

        /**
         * Performs actions to handle the incoming message of a JobUnit.
         * It will need to locate the corresponding DistributableJob and tell it to handle
         * the message.
         *
         * @param id : the JobUnitID of the JobUnit.
         * @param message_number : the number of the message.
         * @param message : the message from the processing client.
         *
         * \sa DistributableJob
         * \sa JobUnit
         */
        virtual void handle_incoming_message_event(JobUnitID id, fud_uint message_number, std::string* message) = 0;

        //DistributableJob events
        /**
         * Handles the completion of a DistributableJob.
         * This means that the Job will no longer produce JobUnits, it can thus be unlinked
         * from the project.
         *
         * @param distjob : The finished DistributableJob.
         *
         * \sa DistributableJob
         */
        virtual void handle_distributable_job_completed_event(DistributableJob* distjob) = 0;

        virtual void quit_event() = 0;
    };

    /**
     * The central hub for jobs in the system.
     * Implements all Job handling functionality.
     *
     * \sa ClientsManagerListener
     * \sa DistributableJobListener
     * \sa JobManagerEventHandler
     */
    class JobManager :
        private ClientsManagerListener,
        private DistributableJobListener,
        private JobManagerEventHandler
    {
        public:
            /**
             * Singleton method.
             */
            static JobManager* get_instance();
            static void destroy();

            /**
             * Returns a pointer to the listener of DistributableJob events.
             *
             * \sa DistributableJobListener
             * \sa DistributableJob
             * \sa Event
             */
            inline DistributableJobListener* get_distributable_job_listener() {return this;}

            /**
             * Enqueues a DistributableJob in the system.
             * The Job doesn't need to be ready to produce, this just means that the framework
             * will be handling it.
             *
             * \sa DistributableJob
             */
            void   enqueue(DistributableJob* distjob);

            /**
             * Start or resume the scheduler thread.
             * It does nothing if the scheduler thread is currently in a running state.
             *
             * \sa stop_scheduler
             */
            void   start_scheduler();

            /**
             * Pauses the scheduler.
             * The scheduler threads continues to run, this only changes its internal state.
             * It will continue to listen for new events, but won't handle them until a call
             * to start_scheduler is invoked.
             *
             * \sa start_scheduler
             */
            void   stop_scheduler();
            
            /**
             * Verify if JobUnit given is incomplete and in this case enqueue to job queue. 
             * @param  id : The JobUnitID to verify
             */
            void    rescue_inclomplete_job_unit(JobUnitID id);

        private:
            /* Override these, as per -Weffc++ warnings */
            JobManager(const JobManager&);
            JobManager& operator=(const JobManager&);

            enum Status {kStopped, kPaused, kRunning};

            JobManager(); 
            virtual ~JobManager();

            /*methods*/
            void              run_scheduler();

            DistributableJob* jobs_available();
            bool              job_queue_full(); //const

            void              create_another_job_unit();

            /* Enqueuing ClientsManager events */
            virtual void free_client_event();
            virtual void job_unit_completed_event(JobUnitID id);
            virtual void incoming_message_event(JobUnitID id, fud_uint message_number, std::string* message);

            /* Enqueuing DistributableJob events */
            virtual void distributable_job_completed_event(DistributableJob* distjob);


            /* handling ClientsManager events */
            virtual void handle_free_client_event();
            virtual void handle_job_unit_completed_event(JobUnitID id);
            virtual void handle_incoming_message_event(JobUnitID id, fud_uint message_number, std::string* message);

            /* handling DistributableJob events */
            virtual void handle_distributable_job_completed_event(DistributableJob* distjob);

            void              handle_new_job_event();

            /* local events*/
            void              job_queue_not_full_event();
            void              handle_job_queue_not_full_event();
            void              quit_event();

            /**
             *  Assign pending job units to a free clients when jobs queue is empty
             */
            void resend_pending_job_unit();

            /* Attr. */
            static JobManager*              _instance;

            ClientsManager*                 _clients_manager;

            std::list<DistributableJob *>          _producingJobs;
            std::list<DistributableJob *>          _waitingJobs;
            std::list<JobUnit *>                   _jobQueue;
            std::list<JobUnit *>                   _pendingList;
            std::map<JobUnitID,DistributableJob* > _ids_to_job_map;

            JobUnitSize                     _current_job_unit_size;

            volatile Status                 _status;

            boost::mutex                    _mutex;
            boost::thread                   _scheduler_thread;

            LockingQueue<Event<JobManagerEventHandler> *>    _event_queue;

    };
}
#endif
