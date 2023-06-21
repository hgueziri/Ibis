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

#include "nonRigidRegistrationwidget.h"
#include <QComboBox>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

#include "ibisapi.h"

NonRigidRegistrationWidget::NonRigidRegistrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NonRigidRegistrationWidget),
    m_pluginInterface(0),
    m_RegistrationRunning(false)
{
    ui->setupUi(this);
    setWindowTitle( "NonRigid Registration" );

    m_FixedCaster = CastFilterType::New();
    m_MovingCaster = CastFilterType::New();
    m_Resampler = ResampleFilterType::New();
    m_NonRigidRegistration = RegistrationMethodType::New();

    m_BackCaster = CastBackFilterType::New();

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);
    ui->progressBar->hide();
    connect(&this->m_FutureWatcher, SIGNAL(finished()), this, SLOT(registration_finished()));

    UpdateUi();
    m_Qout = new QDebugStream(std::cout, ui->registrationOutputTextEdit);
}

NonRigidRegistrationWidget::~NonRigidRegistrationWidget()
{
    delete m_Qout;
    delete ui;
}

void NonRigidRegistrationWidget::SetPluginInterface( NonRigidRegistrationPluginInterface * interf )
{
    m_pluginInterface = interf;
    UpdateUi();
}

void NonRigidRegistrationWidget::on_startButton_clicked()
{
    // Make sure all params have been specified
    int sourceImageObjectId = ui->sourceImageComboBox->itemData( ui->sourceImageComboBox->currentIndex() ).toInt();
    int targetImageObjectId = ui->targetImageComboBox->itemData( ui->targetImageComboBox->currentIndex() ).toInt();

    if(  sourceImageObjectId == -1 || targetImageObjectId == -1 || sourceImageObjectId == targetImageObjectId)
    {
        QMessageBox::information( this, "NonRigid Registration", "Need to specify differente Fixed image and Moving image " );
        return;
    }

    // Get input images
    IbisAPI * api = m_pluginInterface->GetIbisAPI();
    ImageObject * sourceImageObject = ImageObject::SafeDownCast( api->GetObjectByID( sourceImageObjectId ) );
    Q_ASSERT_X( sourceImageObject, "NonRigidRegistrationWidget::on_startButton_clicked()", "Invalid source object" );

    ImageObject * targetImageObject = ImageObject::SafeDownCast( api->GetObjectByID( targetImageObjectId ) );
    Q_ASSERT_X( targetImageObject, "NonRigidRegistrationWidget::on_startButton_clicked()", "Invalid target object" );

    IbisItkFloat3ImageType::Pointer itkSourceImage = sourceImageObject->GetItkImage();
    IbisItkFloat3ImageType::Pointer itkTargetImage = targetImageObject->GetItkImage();

    m_RegistrationTimer.start();
    m_FixedCaster = CastFilterType::New();
    m_FixedCaster->SetInput(itkTargetImage);
    m_FixedCaster->Update();

    m_MovingCaster = CastFilterType::New();
    m_MovingCaster->SetInput(itkSourceImage);
    m_MovingCaster->Update();



    //Set Parameters
    QVariant qv = ui->presetsComboBox->itemData( ui->presetsComboBox->currentIndex() );
    RegistrationParametersType regParameters = qv.value<RegistrationParametersType>();

    if( regParameters.computeMask )
      {
      //TODO: Compute Mask based on Threshold and Erosion
      ui->userFeedbackLabel->setText(QString("Computing Mask.."));
      std::cout << "Computing Mask ...  " << std::endl;
      ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
      thresholder->SetInput(m_FixedCaster->GetOutput());
      thresholder->SetLowerThreshold(1e-3);
      thresholder->SetUpperThreshold(1e15);
      thresholder->SetInsideValue(1.0);
      thresholder->SetOutsideValue(0.0);
      thresholder->Update();

      StructuringElementType structuringElement;
      structuringElement.SetRadius(4);
      structuringElement.CreateStructuringElement();

      ErodeFilterType::Pointer eroder = ErodeFilterType::New();
      eroder->SetInput(thresholder->GetOutput());
      eroder->SetKernel(structuringElement);
      eroder->Update();

      m_NonRigidRegistration->SetFixedMask( eroder->GetOutput() );
      ui->userFeedbackLabel->setText(QString("Computing Mask..Done"));
      std::cout << "Computing Mask ... DONE"   << std::endl;
      // DEBUG
      //Add Mask Volume to Scene
      /*
      m_BackCaster->SetInput( eroder->GetOutput() );
      m_BackCaster->Update();

      ImageObject * maskImage = ImageObject::New();
      maskImage->SetItkImage( m_BackCaster->GetOutput() );
      maskImage->SetName("Mask Image");

      api->AddObject(maskImage, sourceImageObject->GetParent() );
      //*/

      }

    if( sourceImageObject->GetParent() != targetImageObject->GetParent() )
      {
      RigidTransformType::Pointer rigidTransform = GetRigidTransform( sourceImageObject, targetImageObject );
      m_NonRigidRegistration->SetRigidTransform( rigidTransform );      
      } 

    m_NonRigidRegistration->SetNumberOfLevels(regParameters.numberOfLevels);
    for( int i = 0; i < regParameters.numberOfLevels; i++ )
    {
        m_NonRigidRegistration->SetSelectivity(regParameters.selectivity);
        m_NonRigidRegistration->SetKnotSpacing(regParameters.knotSpacing, i);
        m_NonRigidRegistration->SetDistanceVariance(regParameters.distanceVariance, i);
        m_NonRigidRegistration->SetNumberOfVoxels(regParameters.numberOfVoxels, i);
        m_NonRigidRegistration->SetNumberOfIterations(regParameters.numberOfIterations, i );
        m_NonRigidRegistration->SetSamplingMode(regParameters.samplingMode, i);
        m_NonRigidRegistration->SetGradientScale(regParameters.gradientScale, i);
        m_NonRigidRegistration->SetImageSpacing((regParameters.numberOfLevels - i - 1) * 2 + regParameters.imageSpacing, i); //regParameters.imageSpacing, i);
        m_NonRigidRegistration->SetSymmetryEnabled(regParameters.symmetric);
        m_NonRigidRegistration->SetVerbose(true);
    }

    m_NonRigidRegistration->SetFixedImage(m_FixedCaster->GetOutput(), 0);
    m_NonRigidRegistration->SetMovingImage(m_MovingCaster->GetOutput(), 0);
    
    m_NonRigidRegistration->Initialize();

    ui->progressBar->show();
    ui->progressBar->repaint();
    ui->userFeedbackLabel->setText(QString("Registration in process.."));
    ui->userFeedbackLabel->repaint();
    QApplication::flush();

    m_RegistrationRunning = true;
    QFuture<void> future = QtConcurrent::run(m_NonRigidRegistration.GetPointer(), &RegistrationMethodType::StartOptimization);
    this->m_FutureWatcher.setFuture(future);


}

void NonRigidRegistrationWidget::registration_finished()
{

  // Make sure all params have been specified
  int sourceImageObjectId = ui->sourceImageComboBox->itemData( ui->sourceImageComboBox->currentIndex() ).toInt();
  int targetImageObjectId = ui->targetImageComboBox->itemData( ui->targetImageComboBox->currentIndex() ).toInt();

  if(  sourceImageObjectId == -1 || targetImageObjectId == -1 || sourceImageObjectId == targetImageObjectId)
  {
      QMessageBox::information( this, "NonRigid Registration", "Need to specify differente Fixed image and Moving image " );
      return;
  }

  // Get input images
  IbisAPI * api = m_pluginInterface->GetIbisAPI();
  ImageObject * sourceImageObject = ImageObject::SafeDownCast( api->GetObjectByID( sourceImageObjectId ) );
  ImageObject * targetImageObject = ImageObject::SafeDownCast( api->GetObjectByID( targetImageObjectId ) );

  m_RegistrationRunning = false;

  NonRigidTransformType::Pointer nonRigidTransform = m_NonRigidRegistration->GetFinalTransform();
  nonRigidTransform->SetRigidTransform( NULL ); 

  m_Resampler->SetInput( m_MovingCaster->GetOutput());
  m_Resampler->SetTransform( nonRigidTransform );
  m_Resampler->SetUseReferenceImage(true);
  m_Resampler->SetReferenceImage( m_MovingCaster->GetOutput() );
  m_Resampler->Update();

  m_BackCaster->SetInput( m_Resampler->GetOutput() );
  m_BackCaster->Update();

  //Add Registered Volume to Scene
  ImageObject * registeredImage = ImageObject::New();
  registeredImage->SetItkImage( m_BackCaster->GetOutput() );
  registeredImage->SetName("Registered Image");

  api->AddObject(registeredImage, sourceImageObject->GetParent() );
  sourceImageObject->SetHidden(true);

  qint64 regTime = m_RegistrationTimer.elapsed();
  QString feedbackString = QString("Non-Rigid Registration finished in %1 secs").arg(qreal(regTime)/1000.0);
  ui->userFeedbackLabel->setText(feedbackString);

  ui->progressBar->hide();
}

RigidTransformType::Pointer NonRigidRegistrationWidget::GetRigidTransform( ImageObject * movingImageObject, ImageObject * fixedImageObject )
{
  vtkTransform * fixedVtkTransform = vtkTransform::SafeDownCast( fixedImageObject->GetWorldTransform() );
  vtkTransform * movingVtkTransform = vtkTransform::SafeDownCast( movingImageObject->GetWorldTransform() );

  vtkMatrix4x4 * localMatrix = vtkMatrix4x4::New();
  movingVtkTransform->GetInverse(localMatrix); 

  vtkMatrix4x4 * finalMatrix = vtkMatrix4x4::New(); 
  finalMatrix->Multiply4x4( fixedVtkTransform->GetMatrix(), localMatrix, finalMatrix);

  RigidTransformType::OffsetType offset;

  vnl_matrix<double> M(3,3); 

  for(unsigned int i=0; i<3; i++ )
   {
   for(unsigned int j=0; j<3; j++ )
     {
      M[i][j] = finalMatrix->GetElement(i,j);
     }     
    offset[i] = finalMatrix->GetElement(i,3);
    }

  double angleX, angleY, angleZ;
  angleX = vcl_asin(M[2][1]);
  double A = vcl_cos(angleX);
  if( vcl_fabs(A) > 0.00005 )
    {
    double x = M[2][2] / A;
    double y = -M[2][0] / A;
    angleY = vcl_atan2(y, x);

    x = M[1][1] / A;
    y = -M[0][1] / A;
    angleZ = vcl_atan2(y, x);
    }
  else
    {
    angleZ = 0;
    double x = M[0][0];
    double y = M[1][0];
    angleY = vcl_atan2(y, x);
    }

  RigidTransformType::ParametersType params = RigidTransformType::ParametersType(6);
  params[0] = angleX; params[1] = angleY; params[2] = angleZ;

  RigidTransformType::CenterType center;
  center[0] = fixedImageObject->GetItkImage()->GetOrigin()[0] + fixedImageObject->GetItkImage()->GetSpacing()[0] * fixedImageObject->GetItkImage()->GetBufferedRegion().GetSize()[0] / 2.0;
  center[1] = fixedImageObject->GetItkImage()->GetOrigin()[1] + fixedImageObject->GetItkImage()->GetSpacing()[1] * fixedImageObject->GetItkImage()->GetBufferedRegion().GetSize()[1] / 2.0;
  center[2] = fixedImageObject->GetItkImage()->GetOrigin()[2] + fixedImageObject->GetItkImage()->GetSpacing()[2] * fixedImageObject->GetItkImage()->GetBufferedRegion().GetSize()[2] / 2.0;  


  for( unsigned int i = 0; i < 3; i++ )
    {
    params[i+3] = offset[i] - center[i];
    for( unsigned int j = 0; j < 3; j++ )
      {
      params[i+3] += M[i][j] * center[j];
      }
    }

  RigidTransformType::Pointer    rigidTransform  = RigidTransformType::New();
  rigidTransform->SetCenter(center);
  rigidTransform->SetParameters(params);
  return rigidTransform;

}

void NonRigidRegistrationWidget::UpdateUi()
{
    ui->sourceImageComboBox->clear();
    ui->targetImageComboBox->clear();
    ui->presetsComboBox->clear();
    if( m_pluginInterface )
    {

        //Build Presets
        RegistrationParametersType mr2mrRegParameters;
        mr2mrRegParameters.numberOfLevels = 3;
        mr2mrRegParameters.selectivity = 3.0;
        mr2mrRegParameters.knotSpacing = 64.0;
        mr2mrRegParameters.gradientScale = 2.0;
        mr2mrRegParameters.imageSpacing = 2.0;
        mr2mrRegParameters.distanceVariance = 16.0;
        mr2mrRegParameters.samplingMode = SamplingModeType(1);
        mr2mrRegParameters.numberOfIterations = 60;
        mr2mrRegParameters.numberOfVoxels = 16000;
        mr2mrRegParameters.computeMask = false;
        mr2mrRegParameters.symmetric = true;
        ui->presetsComboBox->addItem( "MR T1 to MR T1", QVariant::fromValue(mr2mrRegParameters) );


        RegistrationParametersType us2mrRegParameters;
        us2mrRegParameters.numberOfLevels = 1;
        us2mrRegParameters.selectivity = 2.0;
        us2mrRegParameters.knotSpacing = 64.0;
        us2mrRegParameters.gradientScale = 2.0;
        us2mrRegParameters.imageSpacing = 2.0;
        us2mrRegParameters.distanceVariance = 16.0;
        us2mrRegParameters.samplingMode = SamplingModeType(1);
        us2mrRegParameters.numberOfIterations = 60;
        us2mrRegParameters.numberOfVoxels = 16000;
        us2mrRegParameters.computeMask = true;
        us2mrRegParameters.symmetric = false;
        ui->presetsComboBox->addItem( "iUS to MR T1", QVariant::fromValue(us2mrRegParameters) );


        IbisAPI * api = m_pluginInterface->GetIbisAPI();
        const SceneManager::ObjectList & allObjects = api->GetAllObjects();
        for( int i = 0; i < allObjects.size(); ++i )
        {
            SceneObject * current = allObjects[i];
            if( current != api->GetSceneRoot() && current->IsListable() && !current->IsManagedByTracker())
            {
                if( current->IsA("ImageObject") )
                {
                    ui->targetImageComboBox->addItem( current->GetName(), QVariant( current->GetObjectID() ) );
                    ui->sourceImageComboBox->addItem( current->GetName(), QVariant( current->GetObjectID() ) );
                }
            }
        }

        if( ui->targetImageComboBox->count() == 0 )
          {
            ui->targetImageComboBox->addItem( "None", QVariant(-1) );
          }

        if( ui->sourceImageComboBox->count() == 0 )
        {
            ui->sourceImageComboBox->addItem( "None", QVariant(-1) );        
        }

    }
}

