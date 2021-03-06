/**
 * \file SonarThdViewer.cpp
 * \brief Classe SonarThdViewer
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Application MOOS donnant les consignes de déplacement de l'AUV
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "SonarThdViewer.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
SonarThdViewer::SonarThdViewer()
{
	m_iterations = 0;
	m_timewarp   = 1;
	img1.create(240,320,CV_8UC3);
	img2.create(240,320,CV_8UC3);
	
	img_son_pol.create(360,800,CV_8UC1);
	img_son_cart.create(400,400,CV_8UC1);
	img_sonar.create(400,400,CV_8UC3);
	pol2cart_x.create(400,400,CV_32FC1);
	pol2cart_y.create(400,400,CV_32FC1);
	
	int width = 400;
	int height = 400;
	
	float xc = (float)width / 2.0;
	float yc = (float)height / 2.0;
	
    
	float ad_interval = 0.25056;
	float mag_step = ad_interval / 2.0;
	
    vector<float> vMag;
    vector<float> vAngle;

    for(double angle = 0; angle < height; angle++)
    {
        for(int mag = 0; mag < width ; mag++)
        {
            vAngle.push_back(angle);
            vMag.push_back((float)mag * mag_step);
        }
    }

    vector<double> vX;
    vector<double> vY;

    //cartToPolar(vX, vY, mag, angle, true);
    cv::polarToCart(vMag, vAngle, pol2cart_x, pol2cart_y, true);

/*    for(size_t i = 0; i < mag.size(); i++)
    {
        cout << "Cartesian (" << vX[i] << ", " << vY[i] << ") " << "<-> Polar (" << mag[i] << ", " << angle[i] << ")" << endl;
    }
*/
/*		
	for(int x=1; x < width; x++)
	{
	    for(int y=1; y < height; y++)
	    {
	        Radius = RadiusMin + y;
	        Angle = 2 * CV_PI * x / imgPol->width;
	        x0 = cvRound(CenterX + Radius * cos(Angle));
// Radius = (x0 - xc) / cos(angle)
// 
	        y0 = cvRound(CenterY + Radius * sin(Angle));
//	 
	        cvSet2D(imgPol, y, x, cvGet2D(imgBase,y0,x0));
	    }
	}*/
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

SonarThdViewer::~SonarThdViewer()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool SonarThdViewer::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;

		#if 0 // Keep these around just for template
		string key   = msg.GetKey();
		string comm  = msg.GetCommunity();
		double dval  = msg.GetDouble();
		string sval  = msg.GetString(); 
		string msrc  = msg.GetSource();
		double mtime = msg.GetTime();
		bool   mdbl  = msg.IsDouble();
		bool   mstr  = msg.IsString();
		#endif
		
		//cout << msg.GetKey() << en
		double pool_angle = MOOSDeg2Rad(10.0);
		
		if( msg.GetKey() == "VVV_VX_DESIRED") {
		  vx = msg.GetDouble();
		}
		if( msg.GetKey() == "VVV_HEADING") {
		//  heading = MOOSDeg2Rad(msg.GetDouble());
		}
		if( msg.GetKey() == "VVV_HEADING_RAZOR") {
		  heading_razor = MOOSDeg2Rad(msg.GetDouble());
		  double a = MOOSDeg2Rad(-12.6), b = 0.45, c = MOOSDeg2Rad(-10.5);
		  heading = heading_razor - ( a*sin(heading_razor+b) + c);
		  
		  heading += pool_angle;
		}
		if( msg.GetKey() == "VVV_HEADING_CISCREA") {
		  heading_ciscrea = MOOSDeg2Rad(msg.GetDouble());
		}
		if( msg.GetKey() == "VVV_IMAGE_SIDE") {
		  memcpy(img1.data, msg.GetBinaryData(), img1.rows*img1.step);
		}
		if( msg.GetKey() == "VVV_IMAGE_BOTTOM") {
		  memcpy(img2.data, msg.GetBinaryData(), img2.rows*img2.step);
		}
		if( msg.GetKey() == "SONAR_RAW_DATA") {
		  float angle = 0;
		  MOOSValFromString(angle, msg.GetString(), "bearing");
		  vector<unsigned int> scanline;
		  int nRows, nCols;
		  MOOSValFromString(scanline, nRows, nCols, msg.GetString(), "scanline");
		  //cout << nRows << " " << nCols << endl;
		  uchar * line = img_son_pol.row((int)floorf(angle)).data;
		  for(int i=0; i<scanline.size();++i)
		    line[i]=scanline[i];
		  line = img_son_pol.row(((int)floorf(angle)+1)%img_son_pol.size().height).data;
		  for(int i=0; i<scanline.size();++i)
		    line[i]=scanline[i];
		  //memcpy(img1.data, msg.GetBinaryData(), img1.rows*img1.step);
		
		  float ad_interval = 0.25056;
		  MOOSValFromString(ad_interval, msg.GetString(), "ad_interval");
		//double scale = 60.0;
		  double scale = 8.0;
		  double mag_step = scale * ad_interval / 2.0;
		  
		  for (double alpha = angle-2.0; alpha <angle+2.0; alpha+=0.5){
		  double cos_b = cos(MOOSDeg2Rad(-alpha) + heading );
		  double sin_b = sin(MOOSDeg2Rad(-alpha) + heading );
		  
    vector<unsigned int>::iterator result;
    result = max_element(scanline.begin(), scanline.end());
    
		  int i = distance(scanline.begin(), result);
		    double d = mag_step * i;
		    int x = sin_b*d + 199.5;
		    int y = cos_b*d + 199.5;
		    if (x>=0 && x<400 && y>=0 && y<400)
		      img_son_cart.at<unsigned char>(x,y) = scanline[i];
		  }
		}
	}

	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool SonarThdViewer::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool SonarThdViewer::Iterate()
{
	m_iterations++;
	cv::imshow("video side",img1);
	cv::imshow("video bottom",img2);
	cv::Point2d center(199.5,199.5);
	cv::Point2d dir(center.x + 100.*cos(heading), center.y + 100.*sin(heading));
	//cv::remap(img_son_pol, img_son_cart, pol2cart_x, pol2cart_y, cv::INTER_NEAREST);
//	cv::applyColorMap(img_son_cart, img_sonar, cv::COLORMAP_JET);
	
	cv::cvtColor(img_son_cart, img_sonar, CV_GRAY2RGB);
	/*
	cv::Mat sonar_flt_h, sonar_flt_v, sonar_flt;
	cv::boxFilter(img_sonar, sonar_flt_h, CV_8UC1, cv::Size2i(40,1));
	cv::boxFilter(img_sonar, sonar_flt_v, 8, cv::Size2i(1,40));
	cv::boxFilter(img_sonar, sonar_flt, CV_8UC1, cv::Size2i(40,10));
		
	cv::threshold(sonar_flt_h - sonar_flt, sonar_flt_h, 10, 255, CV_8UC1);
	/*
	 * /*
	for (int i=200; i<399; ++i) {
	  if (sonar_flt_h.at<unsigned int>(i,200) > 180)
	  {sonar_flt_h.at<unsigned int>(i,200) = 0;}
	}*/
	
	img_son_cart = img_son_cart * 0.999 - 2.0;
	
	//std::stringstream ss;
	//ss << "imgsonar/imgsonar_" << m_iterations << ".jpg";
	//cv::imwrite(ss.str(), img_son_cart);
	
	cv::line(img_sonar, center, dir, cv::Scalar(255,100,0), 2.0);
	cv::imshow("Seuil sur sonar", img_sonar /*sonar_flt_h - sonar_flt*/);
//	cv::imshow("sonar",img_son_pol);
	cv::waitKey(10);
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool SonarThdViewer::OnStartUp()
{
	setlocale(LC_ALL, "C");
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
	{
		list<string>::iterator p;
		for(p = sParams.begin() ; p != sParams.end() ; p++)
		{
			string original_line = *p;
			string param = stripBlankEnds(toupper(biteString(*p, '=')));
			string value = stripBlankEnds(*p);

			if(param == "FOO")
			{
				//handled
			}
			
			else if(param == "BAR")
			{
				//handled
			}
		}
	}

	m_timewarp = GetMOOSTimeWarp();
	
	cv::namedWindow("video side");
	cv::namedWindow("video bottom");
	cv::namedWindow("sonar");

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void SonarThdViewer::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
	m_Comms.Register("VVV_IMAGE_SIDE", 0);
	m_Comms.Register("VVV_IMAGE_BOTTOM", 0);
	m_Comms.Register("SONAR_RAW_DATA", 0);
	m_Comms.Register("VVV_HEADING", 0);
	m_Comms.Register("VVV_HEADING_RAZOR", 0);
	m_Comms.Register("VVV_HEADING_CISCREA", 0);
	m_Comms.Register("VVV_VX_DESIRED", 0);
	m_Comms.Register("VVV_VY_DESIRED", 0);
}
