/*
    Copyright (C) 2015-2025, Navaro, All Rights Reserved
    SPDX-License-Identifier: MIT

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/


#include "qoraal/config.h"
#include "qoraal/qoraal.h"
#include "qoraal/common/lists.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_events.h"
#include "qoraal/svc/svc_threads.h"
#include "qoraal/svc/svc_services.h"

static OS_MUTEX_DECL                (_svc_service_mutex) ;
static SVC_SERVICE_T  *             _svc_service_list = 0 ;
static uint32_t                     _cfg_service_cnt = 0 ;
static stack_t                      _svc_service_handlers ;

#define GET_SVC_SERVICE_HANDLE(pservice) \
            (SCV_SERVICE_HANDLE)( ((uintptr_t)pservice - (uintptr_t)_svc_service_list) / sizeof(SVC_SERVICE_T))

static int32_t      _service_start (SVC_SERVICE_T* pservice, SVC_SERVICE_COMPLETE_CB cb, uintptr_t cb_parm) ;
static void         _service_changed (int32_t status, SVC_SERVICE_T* pservice) ;

/**
 * @brief   svc_service_get
 * @note    return the service handle for use with the API.
 *
 * @param[in] id    service id
 *
 * @return              Handle.
 *
 * @svc
 */
SCV_SERVICE_HANDLE
svc_service_get (SVC_SERVICES_T id)
{
    SVC_SERVICE_T* pservice = 0 ;
    unsigned int i ;
    SCV_SERVICE_HANDLE handle = SVC_SERVICE_INVALID_HANDLE ;

    if (!_svc_service_list)  return handle ;

    for (i=0; i<_cfg_service_cnt; i++) {
        pservice = &_svc_service_list[i] ;
        if (pservice->service == id) {
            handle = i ;
            break ;

        }

    }

    return handle ;
}

/**
 * @brief   svc_service_get_by_name
 * @note    return the service handle for use with the API.
 *
 * @param[in] name    text name of the service
 *
 * @return              Handle.
 *
 * @svc
 */
SCV_SERVICE_HANDLE
svc_service_get_by_name (const char * name)
{
    SVC_SERVICE_T* pservice = 0 ;
    unsigned int i ;
    SCV_SERVICE_HANDLE handle = SVC_SERVICE_INVALID_HANDLE ;

    if (!_svc_service_list)  return handle ;

    for (i=0; i<_cfg_service_cnt; i++) {
        pservice = &_svc_service_list[i] ;
        if (strcasecmp(name, pservice->name) == 0) {
            handle = i ;
            break ;

        }

    }

    if (handle == SVC_SERVICE_INVALID_HANDLE) {
        for (i=0; i<_cfg_service_cnt; i++) {
            pservice = &_svc_service_list[i] ;
            uint32_t l = strlen (name) ;
            if (strncasecmp(name, pservice->name, l) == 0) {
                if (handle == SVC_SERVICE_INVALID_HANDLE) {
                    handle = i ;

                } else {
                    handle = SVC_SERVICE_INVALID_HANDLE ;
                    break ;

                }

            }

        }

    }

    return handle ;
}

/**
 * @brief   svc_service_first
 * @note    used with svc_service_next() for services enumeration.
 *
 * @return              Handle.
 *
 * @svc
 */
SCV_SERVICE_HANDLE
svc_service_first (void)
{
    return 0 ;
}

/**
 * @brief   svc_service_first
 * @note    used with svc_service_first() for services enumeration.
 *
 * @param[in] handle   previous handle returned by svc_service_first() or svc_service_next().
 *
 * @return              Handle.
 *
 * @svc
 */
SCV_SERVICE_HANDLE
svc_service_next (SCV_SERVICE_HANDLE handle)
{
    if (_svc_service_list && handle < _cfg_service_cnt-1) {
        return handle + 1 ;

    }

    return SVC_SERVICE_INVALID_HANDLE ;
}

/**
 * @brief   svc_service_service
 * @note
 *
 * @param[in] handle
 *
 * @return              service id for handle.
 *
 * @svc
 */
SVC_SERVICES_T
svc_service_service (SCV_SERVICE_HANDLE handle)
{
    if (_svc_service_list && handle < _cfg_service_cnt) {
        SVC_SERVICE_T* pservice = &_svc_service_list[handle] ;
        return pservice->service ;

    }

    return SVC_SERVICES_INVALID ;
}

static int32_t
_service_status (SVC_SERVICE_T* pservice)
{
    if ((pservice->status > SVC_SERVICE_STATUS_STOPPED) && pservice->ctrl) {
        int32_t status = pservice->ctrl(SVC_SERVICE_CTRL_STATUS, pservice->parm) ;
        if (status >= 0) {
            return status ;

        }

    }

    return pservice->status ;
}

static void
_service_thread_complete(SVC_THREADS_T * service_thread, void* arg)
{
    SVC_SERVICE_T* pservice = (SVC_SERVICE_T*) arg ;

    os_mutex_lock (&_svc_service_mutex) ;
    pservice->status = SVC_SERVICE_STATUS_STOPPED ;
    if (pservice->cb) pservice->cb (pservice->service, pservice->cb_parm) ;
    pservice->cb = 0 ;
    pservice->cb_parm = 0;
    os_mutex_unlock (&_svc_service_mutex) ;

    _service_changed (SVC_SERVICE_STATUS_STOPPED, (pservice)) ;
}


static void
_service_thread (void * arg)
{
    SVC_SERVICE_T* pservice = (SVC_SERVICE_T*) arg ;

    os_mutex_lock (&_svc_service_mutex) ;
    pservice->status = SVC_SERVICE_STATUS_STARTED ;
    if (pservice->cb) pservice->cb (pservice->service, pservice->cb_parm) ;
    pservice->cb = 0 ;
    pservice->cb_parm = 0;
    os_mutex_unlock (&_svc_service_mutex) ;

    _service_changed (SVC_SERVICE_STATUS_STARTED, (pservice)) ;

    DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_INFO, "SVC   : : started '%s' in %dms",
                pservice->name, OS_TICKS2MS(os_sys_ticks() - pservice->exit_status));

    int32_t status  = pservice->run (pservice->parm) ;

    DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_LOG, "SVC   : : stopped '%s' in %dms.",
                pservice->name, OS_TICKS2MS(os_sys_ticks() - pservice->exit_status));

    pservice->exit_status = status ;

}

/**
 * @brief   svc_service_services_init
 * @note    initialise list of count services to manage by the API.
 *          parm in the callback is the service handle
 *
 * @param[in] list   list of services
 * @param[in] count  number of services in list.
 *
 * @return           error
 *
 * @svc
 */
int32_t
svc_service_services_init (SVC_SERVICE_T * list)
{
    SVC_SERVICE_T* pservice ;

    DBG_ASSERT_SVC_SERVICES(list, "svc_service_services_init unexpected!") ;

    os_mutex_init (&_svc_service_mutex) ;
    stack_init (&_svc_service_handlers) ;

    _svc_service_list = list ;

    for (_cfg_service_cnt=0; ; ) {
        pservice = &_svc_service_list[_cfg_service_cnt] ;
        if (pservice->service == SVC_SERVICES_INVALID) {
            break ;

        }

        _cfg_service_cnt++ ;
        if (pservice->ctrl) {
            if (pservice->ctrl(SVC_SERVICE_CTRL_INIT, _cfg_service_cnt-1) != EOK) {
                pservice->status = SVC_SERVICE_STATUS_DISABLED ;
            }

        }

    }

    return EOK ;
}

/**
 * @brief   svc_service_register_handler
 * @note    callback whenever any serve changes status.
 *
 * @param[in] handler    user allocated structure
 * @param[in] fp        callback functions
 *
 *
 * @svc
 */
void
svc_service_register_handler (SVC_SERVICE_HANDLER_T * handler, SVC_SERVICE_CALLBACK_T fp, uintptr_t parm)
{
    os_mutex_lock (&_svc_service_mutex) ;
    stack_remove (&_svc_service_handlers, (plists_t)handler, OFFSETOF(SVC_SERVICE_HANDLER_T, next)) ;
    handler->fp = fp ;
    handler->parm = parm ;
    stack_add_head (&_svc_service_handlers, handler, OFFSETOF(SVC_SERVICE_HANDLER_T, next)) ;
    os_mutex_unlock (&_svc_service_mutex) ;
    return  ;
}

/**
 * @brief   svc_service_unregister_handler
 *
 * @param[in] handler
 *
 * @svc
 */
void
svc_service_unregister_handler (SVC_SERVICE_HANDLER_T * handler)
{
    os_mutex_lock (&_svc_service_mutex) ;
    stack_remove (&_svc_service_handlers, (plists_t)handler, OFFSETOF(SVC_SERVICE_HANDLER_T, next)) ;
    os_mutex_unlock (&_svc_service_mutex) ;
}

static void
_service_changed(int32_t status, SVC_SERVICE_T* pservice)
{
    SVC_SERVICE_HANDLER_T* start ;
    os_mutex_lock (&_svc_service_mutex) ;

    for ( start = (SVC_SERVICE_HANDLER_T*)stack_head (&_svc_service_handlers) ;
        (start!=NULL_LLO)
            ; ) {

        start->fp (pservice->service, status, start->parm) ;
        start = (SVC_SERVICE_HANDLER_T*)stack_next ((plists_t)start, OFFSETOF(SVC_SERVICE_HANDLER_T, next));

    }
    os_mutex_unlock (&_svc_service_mutex) ;


}

/**
 * @brief   svc_service_services_start
 * @note    starts all servies in the list matching startflags.
 *
 * @return           error
 *
 * @svc
 */
int32_t
svc_service_services_start (uint32_t startflags)
{
    unsigned int  i ;
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list) return E_UNEXP ;

    os_mutex_lock (&_svc_service_mutex) ;

    for (i=0; i<_cfg_service_cnt; i++) {
        pservice = &_svc_service_list[i] ;
        if (pservice->flags & startflags) {

            _service_start (pservice, 0, 0) ;

        }

    }

    os_mutex_unlock (&_svc_service_mutex) ;

    return EOK ;
}

/**
 * @brief   svc_service_system_halt
 * @note    called for immediate shutdown.
 *          sends the SVC_SERVICE_CTRL_HALT to all service's control functions.
 *
 *
 *
 * @svc
 */
void
svc_service_system_halt (void)
{
    unsigned int i ;
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list || !_cfg_service_cnt) return ;

    os_mutex_lock (&_svc_service_mutex) ;

    for (i=_cfg_service_cnt; i; i--) {
        pservice = &_svc_service_list[i-1] ;
        if(pservice->status < SVC_SERVICE_STATUS_STARTED) {
            continue ;
        }
        if (pservice->ctrl) {
            pservice->ctrl(SVC_SERVICE_CTRL_HALT, pservice->parm) ;

        }

    }

    os_mutex_unlock (&_svc_service_mutex) ;
}


int32_t
_service_start (SVC_SERVICE_T* pservice, SVC_SERVICE_COMPLETE_CB cb, uintptr_t cb_parm)
{
    int res ;
    uint32_t status ;

    do {
        pservice->cb = 0 ;

        if (!pservice->ctrl) {
            res = E_NOIMPL ;
            break ;

        }

        status = _service_status(pservice) ;
        if (status == SVC_SERVICE_STATUS_STARTED) {
            res = EOK ;
            if (cb) cb (pservice->service, cb_parm) ;
            break ;

        }

        if (status == SVC_SERVICE_STATUS_STOPPING) {
            DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_WARNING,
                    "SVC   :W: starting '%s' while stopping", pservice->name) ;
            // ToDo: recovery handling
            ////pservice->status = SVC_SERVICE_STATUS_STOPPED ;

        }

        if (status != SVC_SERVICE_STATUS_STOPPED) {
            res = E_UNEXP ;
            break ;

        }

        if (pservice->run) {
            if (svc_threads_is_active(&pservice->thd)) {
                res = EFAIL ;
                break ;

            }
        }


        DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_INFO,
                "SVC   : : starting '%s'...", pservice->name) ;
        pservice->exit_status = os_sys_ticks() ;

        if ((res = pservice->ctrl(SVC_SERVICE_CTRL_START, pservice->parm)) != EOK) {
            _service_changed (SVC_SERVICE_STATUS_STOPPED, (pservice)) ;
            break ;

        }

        if (!pservice->run) {
            DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_LOG,
                    "SVC   : : started '%s' in %dms",
                    pservice->name, OS_TICKS2MS(os_sys_ticks() - pservice->exit_status));
            pservice->exit_status = EOK ;

            pservice->status = SVC_SERVICE_STATUS_STARTED ;
            if (cb) cb (pservice->service, cb_parm) ;

            _service_changed (SVC_SERVICE_STATUS_STARTED, (pservice)) ;


            res = EOK ;
            break ;

        }


        pservice->cb = cb ;
        pservice->cb_parm = cb_parm ;
        _service_changed (SVC_SERVICE_STATUS_STARTING, (pservice)) ;
        res = svc_threads_create (&pservice->thd, _service_thread_complete,
                pservice->stack, pservice->prio, _service_thread, (void *)pservice, pservice->name) ;

        pservice->status = res == EOK ? SVC_SERVICE_STATUS_STARTING : SVC_SERVICE_STATUS_STOPPED ;
        if (res != EOK) _service_changed (SVC_SERVICE_STATUS_STOPPED, (pservice)) ;

    } while (0) ;

    if (res != EOK) {
        DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_WARNING,
            "SVC   : : failed with %d starting '%s'",
            res, pservice->name);
    }

    return res ;
}

/**
 * @brief   svc_service_start
 * @note    Starts a specific service. If the service implements the "run" function, the state will change to
 *          "starting". If the run function is
 *
 * @param[in] handle
 * @param[in] cb
 * @param[in] cb_parm
 *
 * @return           error
 *
 * @svc
 */
int32_t
svc_service_start (SCV_SERVICE_HANDLE handle, uintptr_t arg, SVC_SERVICE_COMPLETE_CB cb, uintptr_t cb_parm)
{
    int res = E_UNEXP ;
    int32_t status ;
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list) return E_UNEXP ;
    if (handle >= _cfg_service_cnt) return E_NOTFOUND ;

    os_mutex_lock (&_svc_service_mutex) ;

    pservice = &_svc_service_list[handle] ;
    status = _service_status (pservice) ;

    if (
            (status != SVC_SERVICE_STATUS_RESIDENT) &&
            (status != SVC_SERVICE_STATUS_STARTING) &&
            (status != SVC_SERVICE_STATUS_STOPPING) &&
            (status != SVC_SERVICE_STATUS_DISABLED)
        ) {

        pservice->parm = arg ;
        res = _service_start (pservice, cb, cb_parm) ;

    }

    os_mutex_unlock (&_svc_service_mutex) ;

    return res ;
}

/**
 * @brief   svc_service_stop
 * @note    Stops a specific service.
 *
 * @param[in] handle
 * @param[in] cb
 * @param[in] cb_parm
 *
 * @return           error
 *
 * @svc
 */
int32_t
svc_service_stop (SCV_SERVICE_HANDLE handle, SVC_SERVICE_COMPLETE_CB cb, uintptr_t cb_parm)
{
    int res = EOK ;
    int32_t status ;
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list) return E_UNEXP ;
    if (handle >= _cfg_service_cnt) return E_NOTFOUND ;

    pservice = &_svc_service_list[handle] ;

    os_mutex_lock (&_svc_service_mutex) ;

    do {
        pservice->cb = 0 ;

        if (!pservice->ctrl) {
            res = E_NOIMPL ;
            break ;

        }

        if (pservice->status == SVC_SERVICE_STATUS_STOPPING) {
            DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_REPORT,
                    "SVC   : : '%s' stopping in progress", pservice->name) ;
        }

        status = _service_status(pservice) ;

        if (status == SVC_SERVICE_STATUS_STOPPED) {
            DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_LOG,
                    "SVC   : : '%s' already stopped", pservice->name) ;
            res = EOK ;
            if (cb) cb (handle, cb_parm) ;
            break ;

        }

        if (status < SVC_SERVICE_STATUS_STARTING) {
            pservice->cb = cb ;
            pservice->cb_parm = cb_parm ;
            res = pservice->ctrl (SVC_SERVICE_CTRL_STOP, pservice->parm) ;
            res = E_UNEXP ;
            _service_changed (SVC_SERVICE_STATUS_STOPPED, (pservice)) ;
            break ;

        }

        DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_INFO,
                "SVC   : : stopping '%s'...", pservice->name) ;

        pservice->exit_status = os_sys_ticks() ;

        if (!pservice->run) {
            pservice->status = SVC_SERVICE_STATUS_STOPPED ;
            res = pservice->ctrl (SVC_SERVICE_CTRL_STOP, pservice->parm) ;
            pservice->exit_status = res ;
            if (cb) cb (pservice->service, cb_parm) ;
            DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_LOG,
                    "SVC   : : stopped '%s' with %d.",
                    pservice->name, res);

            _service_changed (SVC_SERVICE_STATUS_STOPPED, (pservice)) ;


            break ;

        }

        pservice->cb = cb ;
        pservice->cb_parm = cb_parm ;
        _service_changed (SVC_SERVICE_STATUS_STOPPING, (pservice)) ;
        res = pservice->ctrl (SVC_SERVICE_CTRL_STOP, pservice->parm) ;
        pservice->status = res == EOK ? SVC_SERVICE_STATUS_STOPPING : SVC_SERVICE_STATUS_STARTED ;
        if (res != EOK) _service_changed (SVC_SERVICE_STATUS_STARTED, (pservice)) ;

    } while (0) ;

    os_mutex_unlock (&_svc_service_mutex) ;
    return res ;
}

static int32_t
restart_cb(SVC_SERVICES_T id, uintptr_t cb_parm)
{
    os_mutex_lock (&_svc_service_mutex) ;
    p_sem_t  sem = (p_sem_t *) cb_parm ;
    os_sem_signal (sem) ;
    os_mutex_unlock (&_svc_service_mutex) ;
    return EOK ;
}

/**
 * @brief   svc_service_start_timeout
 * @note    Start a specific service.
 *
 * @param[in] handle
 * @param[in] timeout
 *
 * @return           error
 *
 * @svc
 */
int32_t
svc_service_start_timeout (SCV_SERVICE_HANDLE handle, uintptr_t arg, uint32_t timeout)
{
    int32_t res =  E_UNEXP;
    OS_SEMAPHORE_DECL (sem) ;
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list)  return E_UNEXP ;
    if (handle >= _cfg_service_cnt) return E_NOTFOUND ;

    pservice = &_svc_service_list[handle] ;

    os_sem_init (&sem, 0) ;

    if (_service_status (pservice) == SVC_SERVICE_STATUS_STOPPED) {
        if ((res = svc_service_start (handle, arg, restart_cb,  (uintptr_t)&sem)) == EOK) {
            res = os_sem_wait_timeout (&sem, OS_MS2TICKS(timeout)) ;
            os_mutex_lock (&_svc_service_mutex) ;
            pservice->cb = 0 ;
            pservice->cb_parm = 0 ;
            os_mutex_unlock (&_svc_service_mutex) ;

        }

    }

    os_sem_deinit (&sem) ;

    if (res != EOK) {
        DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_ERROR,
                "SVC   :E: restarting '%s' failed with %d.",
                pservice->name, res);

    }

    return EOK ;
}

/**
 * @brief   svc_service_start_timeout
 * @note    Start a specific service.
 *
 * @param[in] handle
 * @param[in] timeout
 *
 * @return           error
 *
 * @svc
 */
int32_t
svc_service_stop_timeout (SCV_SERVICE_HANDLE handle, uint32_t timeout)
{
    int32_t res =  E_UNEXP;
    OS_SEMAPHORE_DECL (sem) ;
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list) return E_UNEXP ;
    if (handle >= _cfg_service_cnt) return E_NOTFOUND ;

    pservice = &_svc_service_list[handle] ;

    os_sem_init (&sem, 0) ;

    if (_service_status (pservice) != SVC_SERVICE_STATUS_STOPPED) {
        if ((res = svc_service_stop (handle, restart_cb,  (uintptr_t)&sem)) == EOK) {
            res = os_sem_wait_timeout (&sem, OS_MS2TICKS(timeout)) ;
            os_mutex_lock (&_svc_service_mutex) ;
            pservice->cb = 0 ;
            pservice->cb_parm = 0 ;
            os_mutex_unlock (&_svc_service_mutex) ;


        }

    }

    os_sem_deinit (&sem) ;


    if (res != EOK) {
        DBG_MESSAGE_SVC_SERVICES (DBG_MESSAGE_SEVERITY_ERROR,
                "SVC   :E: stopping '%s' failed with %d.",
                pservice->name, res);

    }


    return EOK ;
}


/**
 * @brief   svc_service_ctrl
 * @note
 *
 * @param[in] handle
 * @param[in] cmd
 * @param[in] parm
 *
 * @return           error
 *
 * @svc
 */
int32_t
svc_service_ctrl (SCV_SERVICE_HANDLE handle, uint32_t cmd, uintptr_t parm)
{
    int res = EOK ;
    int32_t status ;
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list) return E_UNEXP ;
    if (handle >= _cfg_service_cnt) return E_NOTFOUND ;

    DBG_ASSERT_SVC_SERVICES (_svc_service_list,
            "svc_service_ctrl unexpected!") ;

    pservice = &_svc_service_list[handle] ;

    if (!pservice->ctrl) {
        return E_NOIMPL ;

    }

    os_mutex_lock (&_svc_service_mutex) ;

    status = _service_status(pservice) ;

    if (status < SVC_SERVICE_STATUS_STARTING) {
        res = E_UNEXP ;

    } else {
        res = pservice->ctrl (cmd, parm) ;

        if (res == EOK) {
            int32_t new_status = _service_status (pservice) ;
            if (new_status != status) {
                _service_changed (new_status, pservice) ;

            }

        }

    }

    os_mutex_unlock (&_svc_service_mutex) ;

    return res ;
}

/**
 * @brief   svc_service_ctrl_broadcast
 * @note    send the message to all services
 *
 * @param[in] handle
 * @param[in] cmd
 * @param[in] parm
 *
 * @return           error
 *
 * @svc
 */
void
svc_service_ctrl_broadcast (uint32_t cmd, uintptr_t parm)
{
    if (!_svc_service_list)  return  ;

    SCV_SERVICE_HANDLE handle = svc_service_first () ;

    while (handle != SVC_SERVICE_INVALID_HANDLE) {
        svc_service_ctrl (handle, cmd, parm) ;
        handle = svc_service_next (handle) ;

    }

    return  ;
}

/**
 * @brief   svc_service_status
 * @note
 *
 * @param[in] handle
 *
 * @return           error
 *
 * @svc
 */
int32_t
svc_service_status (SCV_SERVICE_HANDLE handle)
{
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list)  return E_UNEXP ;
    if (handle >= _cfg_service_cnt) return E_NOTFOUND ;

    pservice = &_svc_service_list[handle] ;

    return _service_status (pservice) ;
}

uint32_t
svc_service_get_arg (SCV_SERVICE_HANDLE handle)
{
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list)  return 0 ;
    if (handle >= _cfg_service_cnt) return 0 ;

    pservice = &_svc_service_list[handle] ;

    return pservice->parm ;
}

uint32_t
svc_service_get_flags (SCV_SERVICE_HANDLE handle)
{
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list)  return 0 ;
    if (handle >= _cfg_service_cnt) return 0 ;

    pservice = &_svc_service_list[handle] ;

    return pservice->flags ;
}

int32_t
svc_service_exit_status (SCV_SERVICE_HANDLE handle)
{
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list)  return E_UNEXP ;
    if (handle >= _cfg_service_cnt) return E_NOTFOUND ;

    pservice = &_svc_service_list[handle] ;

    return pservice->exit_status ;
}

const char*
svc_service_name (SCV_SERVICE_HANDLE handle)
{
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list) return "(NOT STARTED)" ;
    if (handle >= _cfg_service_cnt) return "(INVALID)" ;

    pservice = &_svc_service_list[handle] ;

    return pservice->name ;
}

const char*
svc_service_status_name (SCV_SERVICE_HANDLE handle)
{
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list)  return "" ;

    if (handle < _cfg_service_cnt) {

        pservice = &_svc_service_list[handle] ;

        switch (_service_status(pservice)) {
        case SVC_SERVICE_STATUS_DISABLED: return "[ ]" ;
        case SVC_SERVICE_STATUS_STOPPED: return "[-]" ;
        case SVC_SERVICE_STATUS_STARTING: return "[.]" ;
        case SVC_SERVICE_STATUS_STOPPING: return "[.]" ;
        case SVC_SERVICE_STATUS_STARTED: return "[+]" ;
        case SVC_SERVICE_STATUS_RESIDENT: return "[r]" ;

        }

    }

    return "[e]" ;
}

const char*
svc_service_status_description (SCV_SERVICE_HANDLE handle)
{
    SVC_SERVICE_T* pservice ;

    if (!_svc_service_list) return "" ;

    if (handle < _cfg_service_cnt) {

        pservice = &_svc_service_list[handle] ;

        switch (_service_status(pservice)) {
        case SVC_SERVICE_STATUS_DISABLED: return "disabled" ;
        case SVC_SERVICE_STATUS_STOPPED: return "stopped" ;
        case SVC_SERVICE_STATUS_STARTING: return "starting" ;
        case SVC_SERVICE_STATUS_STOPPING: return "stopping" ;
        case SVC_SERVICE_STATUS_STARTED: return "started" ;
        case SVC_SERVICE_STATUS_RESIDENT: return "resident" ;

        }

    }

    return "error" ;
}


