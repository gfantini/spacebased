#include <iostream> // cout
#include <fstream> // ifstream
#include <cstdlib> // getenv
#include <cmath>
//#include <ctgmath>

using namespace std;

bool GetPosition(int ch,double& x,double& y,double& z)
{
  ifstream inputFile;
  string CUORE_INSTALL = getenv("CUORE_INSTALL");
  string path = "/pkg/modtiming/ChannelPosition.dat";
  inputFile.open((CUORE_INSTALL+path).c_str() );
  int Channel;
  double X,Y,Z;
  bool found=false;
  while(inputFile.good())
    {
      inputFile >> Channel >> X >> Y >> Z;
      if(Channel == ch){
        x=X;
        y=Y;
        z=Z;
        found = true;
        break;
      }
    }
  inputFile.close();
  return found;
}

void CheckMaximumDistance()
{
  double x1,y1,z1,x2,y2,z2;
  double Distance;
  double MaximumDistance = 0.; // [mm]
  for(int i=1; i<=988; i++)
    {
      for(int j=1; j<=988; j++)
	{
	  cout << "[INFO] Processing channel \t" << i << "\t" << j << "\t Partial:\t" << MaximumDistance << endl;	  
	  if( !GetPosition(i,x1,y1,z1) )cerr << "Could not find position of channel " << i << endl;
	  if( !GetPosition(j,x2,y2,z2) )cerr << "Could not find position of channel " << j << endl;
	  Distance = pow(x1-x2,2) + pow(y1-y2,2) + pow(z1-z2,2);
	  if(Distance > MaximumDistance*MaximumDistance)MaximumDistance = sqrt(Distance);
	}
    }
  // print result on screen
  cout << "The maximum distance is " << MaximumDistance << " [mm]" << endl;
}

int main()
{

  CheckMaximumDistance();
  return 0;
}
