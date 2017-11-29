
#include "gpuvolumereconstructionapitestplugininterface.h"
#include <QtPlugin>
#include <QMessageBox>

#include "vtkTransform.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "application.h"
#include "scenemanager.h"
#include "imageobject.h"
#include "gpu_volumereconstructionplugininterface.h"
#include "gpu_volumereconstruction.h"
#include "usacquisitionobject.h"

GPUVolumeReconstructionAPITestPluginInterface::GPUVolumeReconstructionAPITestPluginInterface()
{
}

GPUVolumeReconstructionAPITestPluginInterface::~GPUVolumeReconstructionAPITestPluginInterface()
{
}

bool GPUVolumeReconstructionAPITestPluginInterface::CanRun()
{
    return true;
}

QWidget * GPUVolumeReconstructionAPITestPluginInterface::CreateFloatingWidget()
{
    ToolPluginInterface * toolPlugin = this->GetApplication()->GetToolPluginByName( "GPU_VolumeReconstruction");
    if( !toolPlugin )
        return 0;
    GPU_VolumeReconstructionPluginInterface *volumeReconstructorPlugin = GPU_VolumeReconstructionPluginInterface::SafeDownCast( toolPlugin );
    Q_ASSERT( volumeReconstructorPlugin );
    GPU_VolumeReconstruction *reconstructor = GPU_VolumeReconstruction::New();

    SceneManager *sm = this->GetSceneManager();
    QList<USAcquisitionObject*> acquisitions;
    sm->GetAllUSAcquisitionObjects( acquisitions );

    int numberOfAcquisitions = acquisitions.count();
    if( numberOfAcquisitions > 0 )
    {
        for( int i = 0; i < numberOfAcquisitions; i++ )
        {
            USAcquisitionObject * acq = acquisitions[i];
            int nbrOfSlices = acq->GetNumberOfSlices();
            reconstructor->CreateReconstructor();
            reconstructor->SetNumberOfSlices( nbrOfSlices );
            reconstructor->SetFixedSliceMask( acq->GetMask() );
            reconstructor->SetUSSearchRadius( 3 );
            reconstructor->SetVolumeSpacing( 1 );
            reconstructor->SetKernelStdDev( 0.5 );
            vtkSmartPointer<vtkMatrix4x4> sliceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New() ;
            vtkSmartPointer<vtkImageData> slice = vtkSmartPointer<vtkImageData>::New();
            for(int i = 0; i < nbrOfSlices; i++)
            {
                acq->GetFrameData( i, slice.GetPointer(), sliceTransformMatrix.GetPointer() );
                reconstructor->SetFixedSlice(i, slice.GetPointer(), sliceTransformMatrix.GetPointer() );
            }

             //Construct ITK Matrix corresponding to VTK Local Matrix
            reconstructor->SetTransform( acq->GetLocalTransform()->GetMatrix() );
            reconstructor->ReconstructVolume();

            vtkSmartPointer<ImageObject> reconstructedImage = vtkSmartPointer<ImageObject>::New();
            reconstructedImage->SetItkImage( reconstructor->GetReconstructedImage() );
            QString volName("ReconstructedVolume");
            volName.append( QString::number( i ) );
            reconstructedImage->SetName(volName);
            sm->AddObject(reconstructedImage.GetPointer(), acq->GetParent()->GetParent() );
            sm->SetCurrentObject( reconstructedImage.GetPointer() );
        }
        return 0;
    }
    QMessageBox::warning( 0, "Error", "No acquisition in scene." );
    return 0;
}