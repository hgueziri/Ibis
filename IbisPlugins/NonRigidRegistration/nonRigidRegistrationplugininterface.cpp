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

#include "nonRigidRegistrationplugininterface.h"
#include "nonRigidRegistrationwidget.h"
#include "application.h"
#include <QtPlugin>

NonRigidRegistrationPluginInterface::NonRigidRegistrationPluginInterface()
{
}

NonRigidRegistrationPluginInterface::~NonRigidRegistrationPluginInterface()
{
}

bool NonRigidRegistrationPluginInterface::CanRun()
{
    return true;
}

QWidget * NonRigidRegistrationPluginInterface::CreateTab()
{
    NonRigidRegistrationWidget * widget = new NonRigidRegistrationWidget;
    widget->SetPluginInterface(this);
    widget->setAttribute(Qt::WA_DeleteOnClose, true);
    return widget;
}

