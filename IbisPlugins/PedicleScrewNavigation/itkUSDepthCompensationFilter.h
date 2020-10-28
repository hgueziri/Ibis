#ifndef __itkUSDepthCompensationFilter_h
#define __itkUSDepthCompensationFilter_h

#include "itkImageToImageFilter.h"
#include "itkObjectFactory.h"
#include "itkImageRegionIterator.h"
#include "itkImageLinearIteratorWithIndex.h"

#include "itkCastImageFilter.h"


namespace itk
{
    template< class TImage >
    class USDepthCompensationFilter : public ImageToImageFilter< TImage, TImage >
    {
    public:
        /** Standard class typedefs. */
        typedef USDepthCompensationFilter Self;
        typedef ImageToImageFilter< TImage, TImage > Superclass;
        typedef SmartPointer< Self > Pointer;

        typedef TImage ImageType;

        /** Method for creation through the object factory. */
        itkNewMacro(Self);
        
        /** Run-time type information (and related methods). */
        itkTypeMacro(USDepthCompensationFilter, ImageToImageFilter);
        //    itkTypeMacro(InternalMaskType, Internal3DImageMaskType);

    protected:
        USDepthCompensationFilter();
        ~USDepthCompensationFilter() {}
        void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

        /** Does the real work. */
        virtual void GenerateData();

    private:

        USDepthCompensationFilter(const Self &); //purposely not implemented
        void operator=(const Self &);  //purposely not implemented

    };
} //namespace ITK


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkUSDepthCompensationFilter.hxx"
#endif


#endif // __itkUSDepthCompensationFilter_h
