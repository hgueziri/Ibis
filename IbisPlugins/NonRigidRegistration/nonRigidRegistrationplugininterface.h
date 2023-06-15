/*=========================================================================
Ibis Neuronav
Copyright (c) Simon Drouin, Anna Kochanowska, Louis Collins.
All rights reserved.
See Copyright.txt or http://ibisneuronav.org/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.
=========================================================================*/
// Thanks to Dante De Nigris for writing this class

#ifndef __NonRigidRegistrationPluginInterface_h_
#define __NonRigidRegistrationPluginInterface_h_

#include "toolplugininterface.h"

class NonRigidRegistrationWidget;

class NonRigidRegistrationPluginInterface : public ToolPluginInterface
{

    Q_OBJECT
    Q_INTERFACES(IbisPlugin)
    Q_PLUGIN_METADATA(IID "Ibis.NonRigidRegistrationPluginInterface" )

public:

    NonRigidRegistrationPluginInterface();
    ~NonRigidRegistrationPluginInterface();
    virtual QString GetPluginName() { return QString("NonRigidRegistration"); }
    bool CanRun();
    QString GetMenuEntryString() { return QString("Non-Rigid Registration"); }

    QWidget * CreateTab();

};

#endif
