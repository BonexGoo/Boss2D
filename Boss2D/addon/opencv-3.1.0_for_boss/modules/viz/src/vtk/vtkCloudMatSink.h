/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
// Authors:
//  * Anatoly Baksheev, Itseez Inc.  myname.mysurname <> mycompany.com
//
//M*/

#ifndef __vtkCloudMatSink_h
#define __vtkCloudMatSink_h

#include BOSS_OPENCV_V_opencv2__core_hpp //original-code:<opencv2/core.hpp>
#include <vtkWriter.h>

namespace cv
{
    namespace viz
    {
        class vtkCloudMatSink : public vtkWriter
        {
        public:
          static vtkCloudMatSink *New();
          vtkTypeMacro(vtkCloudMatSink,vtkWriter)
          void PrintSelf(ostream& os, vtkIndent indent);

          void SetOutput(OutputArray cloud, OutputArray colors = noArray(), OutputArray normals = noArray(), OutputArray tcoords = noArray());

          // Description:
          // Get the input to this writer.
          vtkPolyData* GetInput();
          vtkPolyData* GetInput(int port);

        protected:
          vtkCloudMatSink();
          ~vtkCloudMatSink();

          void WriteData();
          int FillInputPortInformation(int port, vtkInformation *info);

          _OutputArray cloud, colors, normals, tcoords;

        private:
          vtkCloudMatSink(const vtkCloudMatSink&);  // Not implemented.
          void operator=(const vtkCloudMatSink&);  // Not implemented.
        };
    }
}

#endif
