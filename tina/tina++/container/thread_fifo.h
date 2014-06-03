#ifndef TINAPP_CONTAINER_THREADFIFO_H 
#define TINAPP_CONTAINER_THREADFIFO_H

#include <tina++/tina.h>
#include <tina++/thread.h>
#include <tina++/container/circular_buffer.h>
//#include <algorithm>

namespace TURAG {

namespace {

// the STL-find gives compiler errors.
// So we need to define our own.
template<class InputIterator, class T>
InputIterator find_ (InputIterator first, InputIterator last, const T& val) {
    while (first != last) {
        if (*first == val) return first;
        ++first;
    }
    return last;
}

}
	
/// \brief Thread-sicherer FIFO
/// \ingroup Container
///
/// Thread-sicherer Container für ein Producer-Consumer-Szenario mit
/// beliebig vielen Producern und Consumern.
template <typename T, std::size_t size>
class ThreadFifo {
	NOT_COPYABLE(ThreadFifo);

private:
	CircularBuffer<T, size> buffer_;
	Mutex buffer_mutex_;
    ConditionVariable element_queued_;
    ConditionVariable element_removed_;

public:
    /*!
     * \brief Erzeugt ein Thread-FIFO-Objekt.
     */
    explicit ThreadFifo() :
        element_queued_(&buffer_mutex_), element_removed_(&buffer_mutex_)
    { }

    /*!
     * \brief Fügt ein Element ans Ende des Puffers an.
     * \param[in] mail Anzuhängendes Objekt.
     *
     * Diese Funktion blockiert den aufrufenden Thread solange,
     * bis Platz im Puffer ist.
     */
    void post(const T& mail) {
        ConditionVariable::Lock lock(element_removed_);
        while (buffer_.full()) {
            lock.wait();
        }
        buffer_.emplace_back(mail);
        element_queued_.signal();
    }

    /*!
     * \brief Fügt ein Element ans Ende des Puffers an.
     * \param[in] mail Anzuhängendes Objekt.
     * \param[in] time Timeout.
     * \return True wenn das Element angehängt werden konnte, false falls kein Platz
     * im Puffer war.
     *
     * Diese Funktion blockiert den aufrufenden Thread
     * maximal für die angegebene Zeitspanne oder bis Platz im Puffer ist.
     */
    bool post(const T& mail, SystemTime time) {
        ConditionVariable::Lock lock(element_removed_);

        if (buffer_.full()) {
            lock.waitFor(time);
        }
        if (!buffer_.full()) {
            buffer_.emplace_back(mail);
            element_queued_.signal();
            return true;
        } else {
            return false;
        }
    }


    /*!
     * \brief Fügt ein Element ans Ende des Puffers an, wenn es noch nicht
     * existiert.
     * \param[in] mail Anzuhängendes Objekt.
     *
     * Diese Funktion blockiert den aufrufenden Thread solange,
     * bis Platz im Puffer ist. Für die Gleicheitsprüfung wird der
     * ==-Operator verwendet, der für benutzerdefinierte Klassen ggf. überladen
     * werden muss.
     */
    void postUnique(const T& mail) {
        ConditionVariable::Lock lock(element_removed_);

        // if the element is already part of the list
        // it doesn't matter whether the buffer is full
        if (find_(buffer_.begin(), buffer_.end(), mail) != buffer_.end()) {
            return;
        }

        while (buffer_.full()) {
            lock.wait();
        }

        // after locking the mutex again we can't know whether the element is in the
        // list now, so check again
        if (find_(buffer_.begin(), buffer_.end(), mail) != buffer_.end()) {
            return;
        }

        buffer_.emplace_back(mail);
        element_queued_.signal();
    }


    /*!
     * \brief Fügt ein Element ans Ende des Puffers an, wenn es noch nicht
     * existiert.
     * \param[in] mail Anzuhängendes Objekt.
     * \param[in] time Timeout.
     * \return True wenn das Element angehängt werden konnte oder es bereits Teil
     * der Puffers ist, false falls kein Platz
     * im Puffer war.
     *
     * Diese Funktion blockiert den aufrufenden Thread
     * maximal für die angegebene Zeitspanne oder bis Platz im Puffer ist.
     * Für die Gleicheitsprüfung wird der
     * ==-Operator verwendet, der für benutzerdefinierte Klassen ggf. überladen
     * werden muss.
     */
    bool postUnique(const T& mail, SystemTime time) {
        ConditionVariable::Lock lock(element_removed_);

        // if the element is already part of the list
        // it doesn't matter whether the buffer is full
        if (find_(buffer_.begin(), buffer_.end(), mail) != buffer_.end()) {
            return true;
        }

        if (buffer_.full()) {
            lock.waitFor(time);

            // after locking the mutex again we can't know whether the element is in the
            // list now, so check again
            if (find_(buffer_.begin(), buffer_.end(), mail) != buffer_.end()) {
                return true;
            }
        }
        if (!buffer_.full()) {
            buffer_.emplace_back(mail);
            element_queued_.signal();
            return true;
        } else {
            return false;
        }
    }



    /*!
     * \brief Entnimmt dem Puffer das älteste Element.
     * \param[out] mail Pointer auf das Zielobjekt.
     * \param[in] time Timeout.
     * \return True wenn ein Element innerhalb des Timeouts verfügbar war, ansonsten false.
     */
    bool fetch(T* mail, SystemTime time) {
        ConditionVariable::Lock lock(element_queued_);

        if (buffer_.empty()) {
            lock.waitFor(time);
        }
        if (!buffer_.empty()) {
            if (mail) {
                *mail = buffer_.front();
            }
            buffer_.pop_front();
            element_removed_.signal();
            return true;
        } else {
            return false;
        }
    }

    /*!
     * \brief Entnimmt dem Puffer das älteste Element.
     * \param[out] mail Pointer auf das Zielobjekt.
     *
     * Diese Funktion blockiert, bis ein Element verfügbar ist.
     */
    void fetch(T* mail = nullptr) {
        ConditionVariable::Lock lock(element_queued_);
        while (buffer_.empty()) {
            lock.wait();
        }
        if (mail) {
            *mail = buffer_.front();
        }
        buffer_.pop_front();
        element_removed_.signal();
    }

    /*!
     * \brief Wartet auf die Verfügbarkeit eines Elements.
     * \param[out] mail Pointer auf das Zielobjekt.
     *
     * Diese Funktion blockiert, bis ein Element verfügbar ist und gibt dieses
     * zurück. Das Element verbleibt jedoch im Puffer.
     *
     * \warning Diese Funktion sollte nur benutzt werden, wenn es nur einen Consumer-Thread gibt.
     */
    void wait(T* mail = nullptr) {
        ConditionVariable::Lock lock(element_queued_);
        while (buffer_.empty()) {
            lock.wait();
        }
        if (mail) {
            *mail = buffer_.front();
        }
    }

    /*!
     * \brief Wartet auf die Verfügbarkeit eines Elements.
     * \param[in] time Timeout.
     * \return True wenn ein Element innerhalb des Timeouts verfügbar war, ansonsten false.
     *
     * \warning Diese Funktion sollte nur benutzt werden, wenn es nur einen Consumer-Thread gibt.
     */
    bool  wait(SystemTime time) {
        return wait(nullptr, time);
    }

    /*!
     * \brief Wartet auf die Verfügbarkeit eines Elements und gibt dieses zurück.
     * Das Element verbleibt im Container.
     * \param[out] mail Pointer auf das Zielobjekt.
     * \param[in] time Timeout.
     * \return True wenn ein Element innerhalb des Timeouts verfügbar war, ansonsten false.
     *
     * \warning Diese Funktion sollte nur benutzt werden, wenn es nur einen Consumer-Thread gibt.
     */
    bool  wait(T* mail, SystemTime time) {
        ConditionVariable::Lock lock(element_queued_);

        if (buffer_.empty()) {
            lock.waitFor(time);
        }
        if (!buffer_.empty()) {
            if (mail) {
                *mail = buffer_.front();
            }
            return true;
        } else {
            return false;
        }

    }

};
	
}

#endif // TINAPP_CONTAINER_THREADFIFO_H
