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

#ifndef __NonRigidRegistrationWidget_h_
#define __NonRigidRegistrationWidget_h_

#include <QWidget>
#include <QtGui>
#include <QMessageBox>
#include <QFutureWatcher>
#include "ui_nonRigidRegistrationwidget.h"
#include "nonRigidRegistrationplugininterface.h"
#include "scenemanager.h"
#include "sceneobject.h"
#include "imageobject.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"


#include "itkSymbaRegistrationMethod.h"

#include "itkCastImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"


#include "qdebugstream.h"

/* ITK Registration Tools */
typedef itk::SymbaRegistrationMethod<3>    RegistrationMethodType;

typedef RegistrationMethodType::SamplingModeType  SamplingModeType;

typedef RegistrationMethodType::ImageType         DoubleImageType;
typedef RegistrationMethodType::MaskImageType     MaskImageType;

typedef RegistrationMethodType::NonRigidTransformType  NonRigidTransformType;
typedef RegistrationMethodType::RigidTransformType     RigidTransformType;

//Masking
typedef itk::BinaryThresholdImageFilter<DoubleImageType, MaskImageType>
                                                  ThresholdFilterType;

typedef itk::BinaryBallStructuringElement<
  MaskImageType::PixelType, 3>                  StructuringElementType;
typedef itk::BinaryErodeImageFilter<MaskImageType, MaskImageType, StructuringElementType>
                                                  ErodeFilterType;


//Temporary Hack

typedef itk::CastImageFilter<IbisItkFloat3ImageType,DoubleImageType>
                                           CastFilterType;

typedef itk::CastImageFilter<DoubleImageType, IbisItkFloat3ImageType>
                                           CastBackFilterType;

typedef itk::ResampleImageFilter<DoubleImageType, DoubleImageType>
                                            ResampleFilterType;

typedef itk::ImageFileWriter<DoubleImageType>
                                           WriterType;

struct RegistrationParametersType {
  int numberOfVoxels;
  int numberOfIterations;
  SamplingModeType samplingMode;
  double gradientScale;
  double imageSpacing;
  double distanceVariance;
  double knotSpacing;
  unsigned int selectivity;
  bool    computeMask;
  bool    symmetric;
};

Q_DECLARE_METATYPE(RegistrationParametersType);

class Application;

namespace Ui
{
    class NonRigidRegistrationWidget;
}


class NonRigidRegistrationWidget : public QWidget
{

    Q_OBJECT

public:

    explicit NonRigidRegistrationWidget(QWidget *parent = 0);
    ~NonRigidRegistrationWidget();

    void SetPluginInterface( NonRigidRegistrationPluginInterface * interf );

private:

    void UpdateUi();

    RigidTransformType::Pointer     GetRigidTransform( ImageObject * movingImageObject, ImageObject * fixedImageObject );

    Ui::NonRigidRegistrationWidget  * ui;
    NonRigidRegistrationPluginInterface * m_pluginInterface;
    bool                            m_RegistrationRunning;

    CastFilterType::Pointer         m_FixedCaster;
    CastFilterType::Pointer         m_MovingCaster;

    ResampleFilterType::Pointer     m_Resampler;
    RegistrationMethodType::Pointer m_NonRigidRegistration;

    CastBackFilterType::Pointer     m_BackCaster;

    QFutureWatcher<void>            m_FutureWatcher;
    QElapsedTimer                   m_RegistrationTimer;
    QDebugStream                    * m_Qout;

    RegistrationParametersType      m_RegistrationParameters;

private slots:

    void on_startButton_clicked();
    void registration_finished();

protected:
    void closeEvent(QCloseEvent *event)
     {
        if( m_RegistrationRunning )
        {
            event->ignore();
            QMessageBox::information( this, "Registration in process..", "Please wait until end of registration." );
        }
        else
        {
            event->accept();
        }
     }    



};

#endif
