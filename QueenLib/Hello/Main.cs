using System;
using QueenLib;
using System.Threading;
using System.IO;
using System.Diagnostics;

namespace Hello
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			Thread.Sleep(5000);
			Execute ex = new Execute();
			ex.run();
		}		

	}
	
	class Execute
	{
		Queen lib = new Queen();
		Thread upTime;
		
		string timeToString(int time)
		{
			if(time<10) return "0" + time;
			else return time.ToString();
		}
		
		void updateTime()
		{
			while(true)
			{			
				string barText = " SirOS";
				int leBarText = barText.Length;
				for(int i=0;i<(128-leBarText)-9;i++) barText += " ";
				barText += timeToString(DateTime.Now.Hour) + ":";
				barText += timeToString(DateTime.Now.Minute) + ":";
				barText += timeToString(DateTime.Now.Second);
				lib.setUpBar(barText);
				Thread.Sleep(1000);
			}
		}
		
		public void run()
		{
			lib.InputEvent+=new Queen.InputHandler(inputHandler); 
			
			lib.initQueen();
			
			//Aggiornamento ora
			upTime = new Thread(updateTime);
			upTime.Start();
			                    
			lib.quSetTitle("Start Queen");
			//List Execute
			int idFile = 0;
			DirectoryInfo di = new DirectoryInfo("applib/list");
			FileInfo[] rgFiles = di.GetFiles("*.*");
			foreach(FileInfo fi in rgFiles)
			{
				if(!fi.FullName.Contains(".dll"))
				{
					int sx = ((idFile%5)*150)+15, sy = ((idFile/5)*50)+10, lng = (135-(fi.Name.Length*8))/2;
					lib.drawRect(sx,sy-5,sx+(135),sy+(8+5),15);
					lib.qrWrite(sx+lng,sy, fi.Name, 0, 15);				
					
					lib.areaAdd(fi.Name,sx,sy-5,sx+(135),sy+(8+5));  
					idFile++;
				}
			}
			
			int tool = lib.getNewTool();
			lib.setTool(tool,0,"Benvenuto in SirOS");
			lib.setUpBar("SirOS");
			
			Console.ReadKey();
			lib.closeQueen();
		}
		
		private void inputHandler(int typeEvent, int[] ar) {
			
			if(typeEvent==2)
			{
				int area = lib.areaGet(ar[0],ar[1]);
				if(area != -1 && ar[3]==0)
				{
					Process myProc;
					myProc = Process.Start("applib/list/"+lib.areaList[area].name);
				}		
			}
    	} 
	}
}

