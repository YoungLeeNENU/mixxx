/***************************************************************************
                          controlobject.h  -  description
                             -------------------
    begin                : Wed Feb 20 2002
    copyright            : (C) 2002 by Tue and Ken Haste Andersen
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTROLOBJECT_H
#define CONTROLOBJECT_H

#include <qobject.h>
#include <qwidget.h>
#include <qevent.h>
#include <qptrlist.h>
#include <qaccel.h>
#include <qptrqueue.h>
#include <qmutex.h>
#include "configobject.h"
#include "defs.h"
#include "midiobject.h"

class ControlEngine;

struct ControlQueueEngineItem
{
    ControlEngine *ptr;
    double value;
};


/**
  * ControlObjects is used as a way to share controller values between controllers, GUI and
  * the sound engine. Whenever the value is changed by either a connected widget or a ControlEventMidi
  * the emitValueChanged method is called which syncronizes the new value with the player thread
  * using the semaphore protected queue.
  *
  * The player thread has a corresponding ControlEngine object for each ControlObject. The 
  * ControlEngine object updates the ControlObject by queueing the values, and sending them as an
  * event to the ControlObject.
  *
  *@author Tue and Ken Haste Andersen
  */

class ControlObject : public QObject
{
    Q_OBJECT
public:
    ControlObject();
    ControlObject(ConfigKey key);
    ~ControlObject();
    /** Connect two control objects dest and src, so each time src is updated, so is dest */
    bool connectControls(ConfigKey src, ConfigKey dest);
    /** Returns a pointer to a QString containing the ConfigKey */
    QString *print();
    /** Sets the config object */
    static void setConfig(ConfigObject<ConfigValueMidi> *_config);
    /** Associates a QWidget with the ControlObject identified by a given ConfigKey */
    static void setWidget(QWidget *widget, ConfigKey key, bool emitOnDownPress=true, Qt::ButtonState state=Qt::NoButton);
    /** Associates a QWidget with the ControlObject */
    void setWidget(QWidget *widget, bool emitOnDownPress=true, Qt::ButtonState state=Qt::NoButton);
    /** Used to set the corresponding ControlEngine number of this ControlObject */
    void setControlEngine(ControlEngine *pControlEngine);
    /** Return the value of the ControlObject */
    FLOAT_TYPE getValue();
    /** Used to set a keyboard accelarator (up) for the ControlObject. Up and down directions are provided,
      * even though some ControlObjects does not distinguish between the two (ControlPushButton for
      * instance */
    virtual void setAccelUp(const QKeySequence key) = 0;
    /** Used to set a keyboard accelarator (down) for the ControlObject */
    virtual void setAccelDown(const QKeySequence key) = 0;
    /** Sets up parent widget. Used when setting up keyboard accelerators */
    static void setParentWidget(QWidget *pParentWidget);
    /** Syncronizes queue with ControlEngine objects. Non blocking. Should be called from player
      * thread. */
    static void sync();
public slots:
    /** Slot used to update the value */
    virtual void slotSetPositionExtern(float) = 0;
    /** Slot used to update the value from a MIDI event */
    virtual void slotSetPositionMidi(MidiCategory c, int v) = 0;
    /** Set the value of the object. Called from event handler when receiving ControlEventEngine. */
    void setValue(FLOAT_TYPE);

signals:
    /** Signal sent when the widget has to be updated with a given value */
    void updateGUI(float);
    /** Signal sent when the ControlObject value has changed */
    void valueChanged(FLOAT_TYPE);

protected:
    /** Forces the gui to be updated with the value of the controller */
    virtual void forceGUIUpdate() = 0;
    /** Method emit a valueChanged signal and puts the new value in the queue */
    void emitValueChanged(FLOAT_TYPE);
    /** Return pointer to parent widget */
    QWidget *getParentWidget();
    /** The actual value of the controller */
    FLOAT_TYPE value;
    /** Pointer to MIDI config */
    static ConfigObject<ConfigValueMidi> *config;
    /** Queue used in syncronizing the value with the Player thread */
    static QPtrQueue<ControlQueueEngineItem> queue;
    /** Pointer to config option */
    ConfigOption<ConfigValueMidi> *cfgOption;
    /** Pointer to associated ControlEngine object */
    ControlEngine *m_pControlEngine;
    /** Keyboard accelerator */
    QAccel *m_pAccel;


private:
    /** Called when a ControlEventMidi event is received */
    void midi(MidiCategory category, char channel, char control, char value);
    /** Event filter. Used to receive ControlEventMidi and ControlEventEngine events */
    bool eventFilter(QObject *, QEvent *);

    /** Pointer to parent widget, used when setting up keyboard accelerators */
    static QWidget *spParentWidget;
    /** List of ControlObject instantiations */
    static QPtrList<ControlObject> list;
    /** Mutex protecting access to the queue */
    static QMutex queueMutex;

};


#endif

