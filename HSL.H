//HSL.h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef HSLh
#define HSLh

/*
Colour Space convertion is such a simple thing to want to do and yet is presented inacurately or incompletely everywhere.
So, I'm listing what I believe to be the best code I can that converts between RGB colours and HSL (sometimes called HLS).

Colours are specified on computers in terms of the intensity of each of the electron guns in the monitor.
There are three guns, one Red, one Green and one Blue (ultimately this means there are just different coloured dots which is what you get on an LCD screen).
The human eye can only perceive 256 levels of intensity, so a Byte is sufficient to store the intensity value.
Unfortunately, when you want to describe a colour its not very easy to do accurately using RGB.
The HSL Colour Space is supposed to be easier.
H is the Hue or colour; S is the Saturation and L is the Lightness.
The ranges are supposed to be:
0-360 for Hue (degrees) - Red being at 0
0-240 for Saturation and Lightness...
H is undefined when S=0;
The Colour Selector in Microsoft Windows, rewrites the standards (anyone surprised?) and uses:
0-239 for Hue
0-240 for Saturation and Lightness...
H is 160 when S=0;

The following code reproduces the conversion the Microsoft way.
*/

//R,G,B values are from 0 to 255
//H=[0,239], S=[0,240], L=[0,240]
//if S==0, then H=160
class CHSL {
public:
  static void RGB2HSL(BYTE& R, BYTE& G, BYTE& B, BYTE& H, BYTE& S, BYTE& L) {
    R=min(max(0,R),255);
    G=min(max(0,G),255);
    B=min(max(0,B),255);
    BYTE minval=min(min(R,G),B);
    BYTE maxval=max(max(R,G),B);
    DWORD msum =maxval+minval;
    L=(BYTE)(0.5+(msum*240/510.));
    if(maxval==minval) {
      S=0;
      H=160;
    }else{
      float delta=(float)maxval-minval;

      S=(BYTE)(0.5+(240*(delta/((msum<=255) ? msum : (510-msum)))));

      float h;
/*      if(R==maxval) h=40*(  (G-B)/delta);
      else if(G==maxval) h=40*(2+(B-R)/delta);
      else if(B==maxval) h=40*(4+(R-G)/delta);
*/
      if(R==maxval) h=40*(  (G-B)/delta);
      else if(G==maxval) h=40*(2+(B-R)/delta);
      else if(B==maxval) h=40*(4+(R-G)/delta);



      if(h>240) 
		  h-=240.0f;
	  
	  //Added by Vishal Kumar************
	  if(h <0.0)
		  h= h + 240.0f;
	  //*********************************

      H=(BYTE)(0.5+h);
	  
    }
  }

  static void HSL2RGB(BYTE& R, BYTE& G, BYTE& B, BYTE& _H, BYTE& _S, BYTE& _L) {
    _H=min(max(0,_H),239);
    _S=min(max(0,_S),240);
    _L=min(max(0,_L),240);
    float H=_H*360/240.0f;
    float S=_S/240.0f;
    float L=_L/240.0f;
    if(S) {
      float rm2;
      if(_L<=120) rm2=L+  L*S;  
      else        rm2=L+S-L*S;
      float rm1=2*L-rm2;   
      R=ToRGB(rm1, rm2, H+120);   
      G=ToRGB(rm1, rm2, H);
      B=ToRGB(rm1, rm2, H-120);
    }else R=G=B=(BYTE)(0.5+L*255);
  }

  static BYTE ToRGB(float rm1, float rm2, float rh) {
         if(rh>360) rh-=360;
    else if(rh<  0) rh+=360;

         if(rh< 60) rm1=rm1+(rm2-rm1)*rh/60;   
    else if(rh<180) rm1=rm2;
    else if(rh<240) rm1=rm1+(rm2-rm1)*(240-rh)/60;      

    return (BYTE)(0.5+rm1*255);
  }
};

#endif // HSLh

