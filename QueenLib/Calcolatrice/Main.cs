using System;
using QueenLib;

namespace Calcolatrice
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			Execute ex = new Execute();
			ex.run();
		}
	}
	
	class Execute
	{
		Queen lib = new Queen();
		bool open = true;
		string numbSt = "",segn="";
		int numb = 0, tool;
		TextBox txt;
		
		int xr = 0, yr=60; //Cordiate relative
		
		void updateText(string text)
		{
			lib.drawRect(xr,yr-25,xr+100,yr-10,15);
			lib.drawEmptyRect(xr,yr-25,xr+100,yr-10,7);	
			lib.qrWrite(xr+1,yr-22,text,0,15);
		}
		
		public void run()
		{
			lib.InputEvent+=new Queen.InputHandler(inputHandler); 
			
			lib.initQueen();                
			lib.quSetTitle("Calcolatrice");
			
			//Imposta coordinate
			xr = (lib.SizeX-100)/2;
			
			//Toolbox
			tool = lib.getNewTool();
			lib.setTool(tool,0," Calcolatrice");
			
			lib.Button("esci","Esci",xr,10,100,20,0,15);
			
			//Tastiera
			updateText("0");
			
			lib.Button("1","1",xr+00,yr+00,15,20,0,15);
			lib.Button("2","2",xr+20,yr+00,15,20,0,15);
			lib.Button("3","3",xr+40,yr+00,15,20,0,15);
			lib.Button("4","4",xr+00,yr+25,15,20,0,15);
			lib.Button("5","5",xr+20,yr+25,15,20,0,15);
			lib.Button("6","6",xr+40,yr+25,15,20,0,15);
			lib.Button("7","7",xr+00,yr+50,15,20,0,15);
			lib.Button("8","8",xr+20,yr+50,15,20,0,15);
			lib.Button("9","9",xr+40,yr+50,15,20,0,15);
			lib.Button("0","0",xr+20,yr+75,15,20,0,15);
			
			lib.Button("CE","CE",xr+60,yr+00,15,20,0,15);
			lib.Button("+","+",xr+60,yr+25,15,20,0,15);
			lib.Button("-","-",xr+60,yr+50,15,20,0,15);
			lib.Button("=","=",xr+60,yr+75,15,20,0,15);
			
			txt = new TextBox(lib,xr,yr+100,100);
			
			string oldtxt="";
			while(open)
			{
				if(oldtxt != txt.Text) 
				{
					oldtxt = txt.Text;
					lib.setTool(tool,2,txt.Text);
				}
			}
			
			lib.killTool(tool);
			lib.closeQueen();
		}
		
		private void inputHandler(int typeEvent, int[] ar) {
			
			if(typeEvent==2)
			{
				int area = lib.areaGet(ar[0],ar[1]);
				if(area!=-1 && (ar[3]==0 || ar[3]==2))
				{
					string name = lib.areaList[area].name;
					if(name=="esci") open = false;
					if(name=="0"||name=="1"||name=="2"||name=="3"||name=="4"||name=="5"||name=="6"||name=="7"||name=="8"||name=="9")
					{
						numbSt += name;
						updateText(numbSt);
					}
					if(name=="CE")
					{
						updateText("0");
						numbSt = "";
						segn = "";
						numb = 0;
					}
					if(name=="+")
					{
						segn = "+";
						numb = int.Parse(numbSt);
						numbSt = "";
						updateText("0");
					}
					if(name=="-")
					{
						segn = "-";
						numb = int.Parse(numbSt);
						numbSt = "";
						updateText("0");
					}
					if(name=="=")
					{
						int numb2 = int.Parse(numbSt);
						if(segn=="+") numb += numb2;
						if(segn=="-") numb -= numb2;
						if(segn=="") numb = numb2;
						updateText(numb.ToString());
						lib.setTool(tool,1," = " +numb.ToString());
					}
				}
			}
    	}
	}
}

