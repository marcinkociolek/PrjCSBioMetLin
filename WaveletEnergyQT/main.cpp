#include <QCoreApplication>
#include <iostream>
#include <string>
#include <fstream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/contrib/contrib.hpp"

#include <filesystem.hpp>
#include <regex.hpp>

//#include "Functions.h"
#include "RedundantWaveletLib.h"
#include "StringFcLib.h"
#include "DispLib.h"
#include "RegionU16Lib.h"

#include "tinyxml.h"
#include <tinystr.h>

using namespace cv;
using namespace std;
using namespace boost::filesystem;
using namespace boost;

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    // global var declarations
    string OutFileNameExtension = ".bmp";
//    string FileNameExtension = ".bmp";

    bool displayResult = 0;
    bool goThrough = 0;
    bool saveResult = 0;
    bool commonResult = 0;
    bool WavEn = 0;
    // arguments read and decode
    if (argc < 2)
    {
        cout << "\nTo few arguments.";
        return 0;
    }
    string ConfigFileName = argv[1];

    //path OutFolder;

    TiXmlDocument doc(ConfigFileName.c_str());
    doc.LoadFile();

    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);
    TiXmlHandle hFeatures(0);

    pElem = hDoc.FirstChildElement().Element();
    if(!pElem || !pElem->Value())
    {
        cout << "Wrong XML structure";
        return 0;
    }

    string RootStr;
    RootStr = pElem->Value();
    if (RootStr != "FeatureExtraction")
    {
        cout << "Wrong root";
        return 0;
    }

    hRoot = TiXmlHandle(pElem);

    pElem = hRoot.FirstChild("output").FirstChild("directory").Element();
    if(!pElem)
    {
        cout << "No: output -> directory entry";
        return 0;
    }
    if(!pElem->GetText())
    {
        cout << "Empty: output -> directory entry";
        return 0;
    }
    path OutFolder(pElem->GetText());

    pElem = hRoot.FirstChild("inputImages").FirstChild("directory").Element();
    if(!pElem)
    {
        cout << "No: inputImages -> directory entry";
        return 0;
    }
    if(!pElem->GetText())
    {
        cout << "Empty: inputImages -> directory entry";
        return 0;
    }
    path InFolder(pElem->GetText());

    pElem = hRoot.FirstChild("inputImages").FirstChild("pattern").Element();
    if(!pElem)
    {
        cout << "No: inputImages -> pattern entry";
        return 0;
    }
    if(!pElem->GetText())
    {
        cout << "Empty: inputImages -> pattern entry";
        return 0;
    }
    string InFilePattern = pElem->GetText();

    pElem = hRoot.FirstChild("inputPartitions").FirstChild("directory").Element();
    if(!pElem)
    {
        cout << "No: inputPartitions -> directory entry";
        return 0;
    }
    if(!pElem->GetText())
    {
        cout << "Empty: inputPartitions -> directory entry";
        return 0;
    }
    path ROIFolder(pElem->GetText());

    pElem = hRoot.FirstChild("inputPartitions").FirstChild("pattern").Element();
    if(!pElem)
    {
        cout << "No: inputPartitions -> pattern entry";
        return 0;
    }
    if(!pElem->GetText())
    {
        cout << "Empty: inputPartitions -> pattern entry";
        return 0;
    }
    string ROIFilePattern = pElem->GetText();

    //Tiles Handling
    pElem = hRoot.FirstChild("inputGroups").FirstChild("directory").Element();
    if(!pElem)
    {
        cout << "No: inputGroups -> directory entry";
        return 0;
    }
    if(!pElem->GetText())
    {
        cout << "Empty: inputGroups -> directory entry";
        return 0;
    }
    path TileFolder(pElem->GetText());

    pElem = hRoot.FirstChild("inputGroups").FirstChild("pattern").Element();
    if(!pElem)
    {
        cout << "No: inputGroups -> pattern entry";
        return 0;
    }
    if(!pElem->GetText())
    {
        cout << "Empty: inputGroups -> pattern entry";
        return 0;
    }
    string TileFilePattern = pElem->GetText();


    pElem = hRoot.FirstChild("features").Element();
    if(!pElem)
    {
        cout << "No: features entry";
        return 0;
    }

    hFeatures = TiXmlHandle(pElem);

    string FeatureID, ParameterID, ValStr, FeatureID1;
    int maxScale = 10;

    for (int featureNr = 0; featureNr < 1000; featureNr++)
    {
        pElem = hFeatures.Child("feature", featureNr).FirstChild("id").Element();
        if (!pElem)
            break;
        FeatureID = pElem->GetText();
        if (FeatureID == "WaveletFeatures_RedundantWaveletEnergy")
        {
            WavEn = 1;
            pElem = hFeatures.Child("feature", featureNr).FirstChild("parameters").FirstChild("parameter").FirstChild("id").Element();
            if (pElem)
            {
                ParameterID = pElem->GetText();
                if (ParameterID == "MaxScale")
                {
                    pElem = hFeatures.Child("feature", featureNr).FirstChild("parameters").FirstChild("parameter").FirstChild("value").Element();
                    if(pElem && pElem->GetText())
                        maxScale = stoi(pElem->GetText());
                }
            }
        }
    }

    cout << "Output folder name" << "\t" << OutFolder.string() << "\n";
    cout << "Input folder name" << "\t" << InFolder.string() << "\n";
    cout << "Input file pattern" << "\t" << InFilePattern << "\n";

    if (WavEn)
        cout << "calculate wavelet subband Energies" << "\n";
    if (!WavEn)
    {
        cout << "No energies requested " << "\n";
        return 0;
    }
    cout << "MaxScaleSet" << "\t" << maxScale << "\n";
    cout << "\n";
    //cout << hDoc.FirstChildElement().Element()->Value();
    //cout << "\n";
    //cout << hDoc.FirstChildElement("output").Element()->Value();
    cout << "\n";
    //cin >> StrTemp;
    //return 0;

    // output arguments
    string StrTemp;
    if (displayResult)
        StrTemp = "Y";
    else
        StrTemp = "N";
    cout << "Display result images - " << StrTemp << "\n";

    if (saveResult)
        StrTemp = "Y";
    else
        StrTemp = "N";
    cout << "Save result images - " << StrTemp << "\n";

    if (commonResult)
        StrTemp = "Y";
    else
        StrTemp = "N";

    cout << "Save feature calc - " << StrTemp << "\n";
    cout << "\n";

    if (displayResult)
    {
        namedWindow("Image", WINDOW_AUTOSIZE);
        namedWindow("ROI", WINDOW_AUTOSIZE);

        namedWindow("ImageSmall", WINDOW_AUTOSIZE);
        namedWindow("ROISmall", WINDOW_AUTOSIZE);

        namedWindow("ImageLL", WINDOW_AUTOSIZE);
        namedWindow("ImageLH", WINDOW_AUTOSIZE);
        namedWindow("ImageHL", WINDOW_AUTOSIZE);
        namedWindow("ImageHH", WINDOW_AUTOSIZE);

        //namedWindow("ScaleLL", WINDOW_AUTOSIZE);
        //namedWindow("ScaleHL", WINDOW_AUTOSIZE);
        //namedWindow("ScaleLH", WINDOW_AUTOSIZE);
        //namedWindow("ScaleHH", WINDOW_AUTOSIZE);
    }

    // read list of image files

    //create FileList
    if(!exists(InFolder) or !is_directory(InFolder))
    {
        cout << InFolder.string() << " is not a valid input directory";
        cout << "\n\n";
        return 0;
    }

    cout << InFolder.string() << "is a valid input directory";
    cout << "\n\n";

    int count = 0;

    list<string> ImageFileNameList;

    for (directory_entry& fileToProcess : directory_iterator(InFolder))
    {
        cout << count << "\t" << fileToProcess.path().filename().string();

        regex FilePattern(InFilePattern);

        if(!regex_match(fileToProcess.path().filename().string(), FilePattern))
        {
            cout << "  File does not fit the patern  " << "\n";
            continue;
        }

        cout << "\n";
        count++;

        ImageFileNameList.push_back(fileToProcess.path().string());

    }

    ImageFileNameList.sort();

    //create RoiList
    if(!exists(ROIFolder) or !is_directory(ROIFolder))
    {
        cout << ROIFolder.string() << " is not a valid input directory";
        cout << "\n\n";
        return 0;
    }

    cout << ROIFolder.string() << "is a valid input directory";
    cout << "\n\n";

    count = 0;

    list<string> RoiFileNameList;

    for (directory_entry& fileToProcess : directory_iterator(ROIFolder))
    {
        cout << count << "\t" << fileToProcess.path().filename().string();

        regex RoiPattern(ROIFilePattern);

        if(!regex_match(fileToProcess.path().filename().string(), RoiPattern))
        {
            cout << "  File does not fit the patern  " << "\n";
            continue;
        }

        cout << "\n";
        count++;

        RoiFileNameList.push_back(fileToProcess.path().string());

    }

    RoiFileNameList.sort();

    //create TileList
    if(!exists(TileFolder) or !is_directory(TileFolder))
    {
        cout << TileFolder.string() << " is not a valid input directory";
        cout << "\n\n";
        return 0;
    }

    cout << TileFolder.string() << "is a valid input directory";
    cout << "\n\n";

    count = 0;

    list<string> TileFileNameList;

    for (directory_entry& fileToProcess : directory_iterator(TileFolder))
    {
        cout << count << "\t" << fileToProcess.path().filename().string();

        regex TilePattern(TileFilePattern);

        if(!regex_match(fileToProcess.path().filename().string(), TilePattern))
        {
            cout << "  File does not fit the patern  " << "\n";
            continue;
        }

        cout << "\n";
        count++;

        TileFileNameList.push_back(fileToProcess.path().string());

    }

    TileFileNameList.sort();


    list<string>::iterator iRoiFileNameList = RoiFileNameList.begin();
    list<string>::iterator iTileFileNameList = TileFileNameList.begin();

    for (list<string>::iterator iImageFileNameList = ImageFileNameList.begin(); iImageFileNameList !=ImageFileNameList.end(); iImageFileNameList++)
    {
        // read image

        string ImageFileName = *iImageFileNameList;
        path ImageFile(ImageFileName);

        string FileNameBase = ImageFile.filename().stem().string();

        Mat ImIn = imread(ImageFileName, CV_LOAD_IMAGE_ANYDEPTH);

        int maxX, maxY, maxXY;
        maxX = ImIn.cols;
        maxY = ImIn.rows;
        maxXY = maxX * maxY;


        //ROIFile.
        string RoiFileName = *iRoiFileNameList;

        if(iRoiFileNameList !=RoiFileNameList.end())
            iRoiFileNameList++;

        path ROIFile(RoiFileName);
        Mat ImROI;
        bool noPartitions = true;

        if(exists(ROIFile) && is_regular_file(ROIFile))
        {
            ImROI = imread(ROIFile.string(), CV_LOAD_IMAGE_ANYDEPTH);
            noPartitions = false;
        }
        else
        {
            ImROI = Mat::ones(maxY, maxX, CV_16U);
            cout << "no Mask File default mask is created" << "\n";
        }
        ImROI.convertTo(ImROI, CV_16U);

        //TileFile.
        string TileFileName;
        bool noTiles = true;
        Mat ImTile;
        path TileFile;

        if(iTileFileNameList !=TileFileNameList.end())
        {
            TileFileName = *iTileFileNameList;
            iTileFileNameList++;

            TileFile =path(TileFileName);
        }


        if(exists(TileFile) && is_regular_file(TileFile))
        {
            ImTile = imread(TileFile.string(), CV_LOAD_IMAGE_ANYDEPTH);
            noTiles = false;
        }
        else
        {
            cout << "no Tile File " << "\n";
        }
        if(!noTiles)
            ImTile.convertTo(ImTile, CV_16U);


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// add controll mask file size


         // conversion to float
        Mat ImInF;
        ImIn.convertTo(ImInF, CV_32F);

        //show images
        Mat ImTemp, ImShow, ImScale;
        if (displayResult)
        {
            ImShow = ShowImageF32PseudoColor(ImInF,0.0, 256.0);
            imshow("Image", ImShow);

            //ImTemp = Mat::zeros(maxY, maxX, CV_8UC3);
            //ImIn.convertTo(ImTemp, CV_8UC3);
            ImShow = ShowSolidRegionOnImage(GetContour5(ImROI), ImShow);
            //ImShow = ShowImageF32PseudoColor(ImRoiF,0.0, 32.0);
            imshow("ROI", ImShow);
            if(goThrough)
                waitKey(100);
            else
                waitKey(0);
        }
        // output string
        string OutString= "";//File Name: " + fileToProcess.path().stem().string() + "\n";
        OutString += "Partition_ID,Group_ID,";
        for(int s = 1; s <= maxScale; s++)
        {
            OutString += "WaveletFeatures_RedundantWaveletEnergy_bandLL_scale" + ItoStrLZ(s,2) + ",";
        }
        for(int s = 1; s <= maxScale; s++)
        {
            OutString += "WaveletFeatures_RedundantWaveletEnergy_bandLH_scale" + ItoStrLZ(s,2) + ",";
        }
        for(int s = 1; s <= maxScale; s++)
        {
            OutString += "WaveletFeatures_RedundantWaveletEnergy_bandHL_scale" + ItoStrLZ(s,2) + ",";
        }
        for(int s = 1; s <= maxScale; s++)
        {
            OutString += "WaveletFeatures_RedundantWaveletEnergy_bandHH_scale" + ItoStrLZ(s,2) + ",";
        }
        OutString += "\n";

        // ROI
        int *minRoiXPos = new int[65536];
        int *maxRoiXPos = new int[65536];
        int *minRoiYPos = new int[65536];
        int *maxRoiYPos = new int[65536];

        unsigned short maxRoiNr = RoiPositions(ImROI, minRoiXPos, maxRoiXPos, minRoiYPos, maxRoiYPos);

// tile handle to discus!!!!!

        int tile = 0;
        for(int roi = 1; roi <=maxRoiNr; roi++)
        {
            // cut small image and ROI
            Mat SmallIm, SmallROI;

            int positionX = minRoiXPos[roi];
            int positionY = minRoiYPos[roi];

            int maxXSmall = maxRoiXPos[roi] - minRoiXPos[roi];
            int maxYSmall = maxRoiYPos[roi] - minRoiYPos[roi];

            ImInF(Rect(positionX, positionY, maxXSmall, maxYSmall)).copyTo(SmallIm);
            ImROI(Rect(positionX, positionY, maxXSmall, maxYSmall)).copyTo(SmallROI);

            // show small image
            if (displayResult)
            {
                //SmallIm.convertTo(ImTemp, CV_8U);
                //applyColorMap(ImTemp, ImShow, COLORMAP_JET);
                ImShow = ShowImageF32PseudoColor(SmallIm,0.0, 255.0);
                imshow("ImageSmall", ImShow);

                //Mat ImROITemp = SmallROI / roi;
                //ImROITemp.convertTo(ImTemp, CV_8U);
                //ImTemp = ImTemp * 100;
                //applyColorMap(ImTemp, ImShow, COLORMAP_JET);

                //ImTemp = Mat::zeros(SmallIm.rows,SmallIm.cols, CV_8UC3);
                //ImIn.convertTo(ImTemp, CV_8UC3);
                ImShow = ShowSolidRegionOnImage(SmallROI, ImShow);
                imshow("ROISmall", ImShow);

                if(goThrough)
                    waitKey(100);
                else
                    waitKey(0);
            }

            // Strings for output

            string WavMeanEnLL, WavMeanEnHL, WavMeanEnLH, WavMeanEnHH;
            WavMeanEnLL = "";
            WavMeanEnHL = "";
            WavMeanEnLH = "";
            WavMeanEnHH = "";

            //calculate filters
            int oldFilterSize = 0;
            int scale;

            for (scale = 1; scale <= 10; scale++)
            {
                Mat FilterLL, FilterLH, FilterHL, FilterHH;
                int filterSize = (int)pow(2, scale);
                int erodeKernelSize = filterSize - oldFilterSize + 1;
                oldFilterSize = filterSize;
                if ((filterSize > maxXSmall / 2) | (filterSize > maxYSmall / 2))
                    break;

                Mat ErodeKernel = Mat::ones(erodeKernelSize, erodeKernelSize, CV_16U);

                erode(SmallROI, SmallROI, ErodeKernel, Point(-1, -1), 1, BORDER_CONSTANT, 0);

                FilterLL = RedundantWaveletLLFilter(scale);
                FilterHL = RedundantWaveletHLFilter(scale);
                FilterLH = RedundantWaveletLHFilter(scale);
                FilterHH = RedundantWaveletHHFilter(scale);

                // filtering
                Mat ImLL, ImLH, ImHL, ImHH;
                filter2D(SmallIm, ImLL, CV_32F, FilterLL, Point(-1, -1), 0, BORDER_REPLICATE);
                filter2D(SmallIm, ImHL, CV_32F, FilterHL, Point(-1, -1), 0, BORDER_REPLICATE);
                filter2D(SmallIm, ImLH, CV_32F, FilterLH, Point(-1, -1), 0, BORDER_REPLICATE);
                filter2D(SmallIm, ImHH, CV_32F, FilterHH, Point(-1, -1), 0, BORDER_REPLICATE);


                //energy calculations

                float energy;

                energy = MeanEnergyCalculaton(ImLL, SmallROI, roi);
                WavMeanEnLL += std::to_string(energy) + ",";

                energy = MeanEnergyCalculaton(ImLH, SmallROI, roi);
                WavMeanEnLH += std::to_string(energy) + ",";

                energy = MeanEnergyCalculaton(ImHL, SmallROI, roi);
                WavMeanEnHL += std::to_string(energy) + ",";

                energy = MeanEnergyCalculaton(ImHH, SmallROI, roi);
                WavMeanEnHH += std::to_string(energy) + ",";



                // display and save images
                if (displayResult || saveResult)
                {
                    //int maxIntLL = 255;
                    //int maxIntHL = 255;
                    //int maxIntLH = 255;
                    //int maxIntHH = 255;


                    Mat EnImLL, EnImLH, EnImHL, EnImHH;
                    EnImLL = MeanEnergyImage(ImLL, SmallROI, roi);
                    EnImHL = MeanEnergyImage(ImHL,  SmallROI, roi);
                    EnImLH = MeanEnergyImage(ImLH,  SmallROI, roi);
                    EnImHH = MeanEnergyImage(ImHH,  SmallROI, roi);


                    //double minSubband, maxSubband;

                    //LL band
                    //ImShow = ImageInPseudocolors(EnImLL, 2, 0, maxIntLL, &minSubband, &maxSubband);
                    ImShow = ShowImageF32PseudoColor(EnImLL,0.0, 255.0);
                    MaskImageInPseudocolors(ImShow, SmallROI, 200);
                    //ImScale = PrepareColorScale(minSubband, maxSubband, 100);

                    if (displayResult)
                    {
                        imshow("ImageLL", ImShow);
                      //  imshow("ScaleLL", ImScale);
                    }
                    if (saveResult)
                    {
                        string OutFileName = OutFolder.string() + FileNameBase + "Roi" + ItoStrLZ(roi,5) + "Band" + "LL" + "Scale" + ItoStrLZ(scale,2) + OutFileNameExtension;
                        imwrite(OutFileName, ImShow);
                    }
                    //HL band
                    //ImShow = ImageInPseudocolors(EnImHL, 2, 0, maxIntHL, &minSubband, &maxSubband);
                    ImShow = ShowImageF32PseudoColor(EnImHL,0.0, 255.0);
                    MaskImageInPseudocolors(ImShow, SmallROI, 200);
                    //ImScale = PrepareColorScale(minSubband, maxSubband, 100);
                    if (displayResult)
                    {
                        imshow("ImageHL", ImShow);
                        //imshow("ScaleHL", ImScale);
                    }
                    if (saveResult)
                    {
                        string OutFileName = OutFolder.string() + FileNameBase + "Roi" + ItoStrLZ(roi,5) + "Band" + "HL" + "Scale" + ItoStrLZ(scale,2) + OutFileNameExtension;
                        imwrite(OutFileName, ImShow);
                    }
                    //LH band
                    //ImShow = ImageInPseudocolors(EnImLH, 2, 0, maxIntLH, &minSubband, &maxSubband);
                    ImShow = ShowImageF32PseudoColor(EnImLH,0.0, 256.0);
                    MaskImageInPseudocolors(ImShow, SmallROI, 200);
                    //ImScale = PrepareColorScale(minSubband, maxSubband, 100);
                    if (displayResult)
                    {
                        imshow("ImageLH", ImShow);
                        //imshow("ScaleLH", ImScale);
                    }
                    if (saveResult)
                    {
                        string OutFileName = OutFolder.string()  + FileNameBase + "Roi" + ItoStrLZ(roi,5) + "Band" + "LH" + "Scale" + ItoStrLZ(scale,2) + OutFileNameExtension;
                        imwrite(OutFileName, ImShow);
                    }
                    //HH band
                    //ImShow = ImageInPseudocolors(EnImHH, 2, 0, maxIntHH, &minSubband, &maxSubband);
                    ImShow = ShowImageF32PseudoColor(EnImLL,0.0, 256.0);
                    MaskImageInPseudocolors(ImShow, SmallROI, 200);
                    //ImScale = PrepareColorScale(minSubband, maxSubband, 100);
                    if (displayResult)
                    {
                        imshow("ImageHH", ImShow);
                        //imshow("ScaleHH", ImScale);
                    }
                    if (saveResult)
                    {
                        string OutFileName = OutFolder.string()  + FileNameBase + "Roi" + ItoStrLZ(roi,5) + "Band" + "HH" + "Scale" + ItoStrLZ(scale,2) + OutFileNameExtension;
                        imwrite(OutFileName, ImShow);
                    }

                    if (displayResult)
                    {
                        if(goThrough)
                            waitKey(100);
                        else
                            waitKey(0);
                    }
                }
            }
            for (int s = scale; s <= 10; s++)
            {
                WavMeanEnLL += "NaN,";
                WavMeanEnLH += "NaN,";
                WavMeanEnHL += "NaN,";
                WavMeanEnHH += "NaN,";
            }

            if(!noPartitions)
            {
                OutString += std::to_string(roi) + ",";
            }
            else
            {
                OutString += std::to_string(0) + ",";
            }
            OutString += std::to_string(tile) + ",";
            OutString += WavMeanEnLL;
            OutString += WavMeanEnHL;
            OutString += WavMeanEnLH;
            OutString += WavMeanEnHH;
            OutString += "\n";

        }
        //cout << OutString << "\n";
        // save energies file
        path OutFile = OutFolder;
        OutFile.append(ImageFile.filename().string() + ".csv");

        ofstream out(OutFile.string());
        out << OutString;
        out.close();
    }

    cout << "\n" << "# of files " << count <<"\n";
    //handle files

    return 0;
}
