#ifndef __itkUSDepthCompensationFilter_hxx
#define __itkUSDepthCompensationFilter_hxx

#include "itkUSDepthCompensationFilter.h"

namespace itk
{

    template< typename TImage >
    USDepthCompensationFilter< TImage >
        ::USDepthCompensationFilter()
    {
        /// deprecated: used for ReadMaskFile
    
    }

    template< class TImage >
    void USDepthCompensationFilter< TImage >
        ::GenerateData()
    {

        typename ImageType::Pointer input = ImageType::New();
        input->Graft(const_cast<ImageType *>(this->GetInput()));

        typename ImageType::RegionType region = input->GetLargestPossibleRegion();
        typename ImageType::SizeType size = region.GetSize();

        typename ImageType::IndexType index;
        typedef itk::ImageLinearIteratorWithIndex< ImageType > IteratorType;
        IteratorType iter(input, region);
        iter.SetDirection(1);

        for( iter.GoToBegin(); !iter.IsAtEnd(); iter.NextLine() )
        {
            iter.GoToBeginOfLine();
            while( !iter.IsAtEndOfLine() )
            {
                index = iter.GetIndex();
                double dist = (double)(index[1] + 1) / (double)size[1];
                double weight =  std::exp(-dist / 2.0 * 2.0);
                typename ImageType::PixelType px = iter.Get();
                iter.Set(px * weight);
                ++iter;
            }
        }

        this->GraftOutput(input);
    }

    template< typename TImage >
    void
        USDepthCompensationFilter< TImage >
        ::PrintSelf(std::ostream & os, Indent indent) const
    {
        Superclass::PrintSelf(os, indent);
    }

}// end namespace


#endif
