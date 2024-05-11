using System;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace QueenLib
{	
	public class Queen
	{
		//Costanti
		static int AREAMEMORY = 256;		
		
		//Evento
        public delegate void InputHandler(int typeEvent, int[] ar);
        public event InputHandler InputEvent;
		
		//Proprietà
		int background = 0;
		
		public int Background
		{
			get
			{
				return background;	
			}
			set
			{
				background = value;	
			}
		}
		
		public int SizeX
	    {
	        get
	        {
	            return 874;
	        }
	    }
		public int SizeY
	    {
	        get
	        {
	            return 728;
	        }
	    }
		
		public Queen ()
		{
		}	

		//Struct Area
		public struct areaStruct{
		    public bool enable;
		    public string name;
		    public int x1;
		    public int y1;
		    public int x2;
		    public int y2;
		};
		public areaStruct[] areaList = new areaStruct [AREAMEMORY];
		
		//Vars
		public int idCon=1, refe=0;
		string bufferRet;
		Thread srv;
		
		char[] splitChar(int numb) //Dividere int in due char
		{
			char[] splitNumb = new char[2];
		    splitNumb[0] = (char)(numb / 128);
		    splitNumb[1] = (char)(numb % 128);
			
			return splitNumb;
		}
		
		public void sendByte(string toSend, bool isRet)
		{
			int go=10;
			while(go>0)
			{
				try
				{
					//Crea un TcpClient per comunicare col server
				    TcpClient server = new TcpClient();
				    //Si connette al server
				    server.Connect("127.0.0.1", 24000);
				    //Ottiene lo Stream per comunicare col server
				    NetworkStream stream = server.GetStream();
				    //Invia dati al server
				    byte[] data = Encoding.ASCII.GetBytes(toSend);
				    stream.Write(data, 0, data.Length);
					if(isRet==true)
					{
						data = new byte[1024];
						stream.Read(data,0,data.Length);
						bufferRet = Encoding.ASCII.GetString(data);
					}
					
					go=0;
				}
				catch{go--; Thread.Sleep(5);}
			}
		}
		
		void server() {
			//Crea un listener che attende connessioni TCP sulla porta 1234
		    TcpListener listener = new TcpListener(24000+idCon);
		    //Avvia l'attesa di connessioni
		    listener.Start();
			
			while(true)
			{
			    //Accetta il primo TcpClient con richiesta pendente
			    TcpClient client = listener.AcceptTcpClient();
			    //Ottiene lo Stream da usare per la connessione fra client e server
			    NetworkStream stream = client.GetStream();
			    //Invia dei dati
			    byte[] buffer =  new byte[7];
				stream.Read(buffer,0,buffer.Length);
				
				if(buffer[0]==1)
	            {
	                int[] argt = {(int)buffer[1],0,0,0};
	                InputEvent(1,argt);
	            }
	
	            if(buffer[0]==2)
	            {
	                byte xc1 = buffer[2], xc2 = buffer[3], yc1 = buffer[4], yc2 = buffer[5];
	                int xp = ((int)xc1) * 128 + ((int)xc2);
	                int yp = ((int)yc1) * 128 + ((int)yc2);
	
	                int[] argt = {xp,yp,buffer[1],buffer[6]};
					InputEvent(2,argt);
	            }
				
				stream.Close();
				client.Close();
			}
		}
		
		
		public int initQueen()
		{
		    refe=0;
		    sendByte("01\0",true);
		    idCon = int.Parse(bufferRet);
		   				
			srv = new Thread(server);
			srv.Start();
			
		    return refe;
		}
		
		public void closeQueen()
		{
		    sendByte("02"+(char)idCon+"\0",false);
		}
		
		public void quSetTitle(string title)
		{
		    sendByte("03"+(char)idCon+title+"\0",false);
		}
		
		public int[] getMouse()
		{
			int[] ret = new int[2];

			sendByte("08\0",true);
			char[] calcCh = bufferRet.ToCharArray();
			ret[0] = ((int)calcCh[0]*128)+(int)calcCh[1];
			ret[1] = ((int)calcCh[2]*128)+(int)calcCh[3];
			
			return ret;
		}
		
		//Strumenti di disegno
		public int quHereYouAre()
		{
		    refe=0;
		    sendByte("04\0",true);
		    if(refe==0)return bufferRet[0];
		    else return -1;
		}
		
		public void qrWrite(int x, int y, string text, int col, int bg)
		{
		    char[] xp, yp;
		    xp = splitChar(x);
		    yp = splitChar(y);
		
		    sendByte("05"+(char)idCon+xp[0]+xp[1]+yp[0]+yp[1]+(char)col+(char)bg+text+"\0",false);
		}
		
		public void qrLine(int x1, int y1, int x2, int y2, int col)
		{
		    char[] xp1, yp1, xp2, yp2;
		    xp1 = splitChar(x1);
		    yp1 = splitChar(y1);
		    xp2 = splitChar(x2);
		    yp2 = splitChar(y2);
		
		    sendByte("06"+(char)idCon+xp1[0]+xp1[1]+yp1[0]+yp1[1]+xp2[0]+xp2[1]+yp2[0]+yp2[1]+(char)col+"\0",false);
		}
		
		public void qrPoint(int x, int y, int col)
		{
		    char[] xp, yp;
		    xp = splitChar(x);
		    yp = splitChar(y);
		
		    sendByte("07"+(char)idCon+xp[0]+xp[1]+yp[0]+yp[1]+(char)col+"\0",false);
		}
		
		//Gestione area
		public void areaAdd(string name, int x1, int y1, int x2, int y2)
		{
		    int i;
		    for(i=0;i<AREAMEMORY;i++) if(areaList[i].enable!=true)break;
		    if(i<AREAMEMORY)
		    {
		        areaList[i].enable = true;
		        areaList[i].x1 = x1;
		        areaList[i].x2 = x2;
		        areaList[i].y1 = y1;
		        areaList[i].y2 = y2;
				areaList[i].name = name;
		    }
		}
		
		public void areaDel(string name)
		{
		    int i;
		    for(i=0;i<AREAMEMORY;i++)if(areaList[i].enable == true && name == areaList[i].name) areaList[i].enable = false;
		}
		
		public int areaGet(int x, int y)
		{
		    int i;
		    for(i=0;i<AREAMEMORY;i++)if(areaList[i].enable == true)
		    {
		        if((x >= areaList[i].x1 && x <= areaList[i].x2) && (y >= areaList[i].y1 && y <= areaList[i].y2)) return i;
		    }
		
		    return -1;
		}
		
		//Creazione rettangolo
		public void drawEmptyRect(int x1, int y1, int x2, int y2, int col)
		{
		    qrLine(x1,y1,x1,y2,col);
		    qrLine(x1,y1,x2,y1,col);
		    qrLine(x2,y2,x1,y2,col);
		    qrLine(x2,y2,x2,y1,col);
		}
		
		public void drawRect(int x1, int y1, int x2, int y2, int col)
		{
		    int i;
		    for(i=y1;i<=y2;i++) qrLine(x1,i,x2,i,col);
		}
		
		//Gestione toolbox
		public int getNewTool()
		{
		    sendByte("09\0",true);
		    return int.Parse(bufferRet);
		}
		
		public void killTool(int tool)
		{
			sendByte("10"+(char)tool+"\0",false);
		}
		
		public void setTool(int tool,int line, string text)
		{
			sendByte("11"+(char)tool+(char)line+text+"\0",false);
		}
		
		//Gestione barra superiore
		public void setUpBar(string text) //Text: 128 max
		{
			sendByte("12"+text+"\0",false);	
		}
		
		//Creazione componenti grafici semplici
		public void Button(string name, string text, int x, int y, int width, int height, int col, int bg)
		{
			int xt = (width-(text.Length*8))/2, yt = (height-8)/2;
			drawRect(x,y,x+width,y+height,bg);
			areaAdd(name,x,y,x+width,y+height);	
			qrWrite(x+xt,y+yt,text,col,bg);	
		}
		
		//Buckup and restore
		public int Buckup()
		{
			sendByte("13\0",true);	
			return int.Parse(bufferRet);
		}
		
		public void Restore(int id)
		{
			sendByte("14"+(char)id+"\0",false);
		}
	}
	
	//Controlli generici
	public class Button
	{
		Queen qu;
		int x, y, width, height, col, bg;
		string text, name;
		
		//Eventi
		public delegate void ButtonHandler(Button sender,int typeEvent,int modeEvent);
        public event ButtonHandler ButtonClick;
		
		private void inputHandler(int typeEvent, int[] ar) {
			if(typeEvent==2)
			{
				int area = qu.areaGet(ar[0],ar[1]);
				if(area != -1)if(qu.areaList[area].name == name); //ButtonClick(this,ar[3],ar[4]);
			}
		}
		
		void Init()
		{
			name = "but."+ x + "." +y;
			
			int xt = (width-(text.Length*8))/2, yt = (height-8)/2;
			qu.drawRect(x,y,x+width,y+height,bg);
			qu.areaAdd(name,x,y,x+width,y+height);	
			qu.qrWrite(x+xt,y+yt,text,col,bg);	
		}
		
		void Des()
		{
			qu.areaDel(name);
			qu.drawRect(x,y,x+width,y+height,qu.Background);
		}
		
		~ Button()
		{
		    Des();
		}
		
		public Button(Queen qun, string textn, int xn, int yn)
		{
			qu = qun;
			text = textn;
			x = xn;
			y = yn;
			
			//Previsione
			height = 16;
			width = (text.Length*8)+16;
			col = qu.Background;
			bg = 15 - col;
			
			qun.InputEvent+=new Queen.InputHandler(inputHandler); 
			
			Init();
		}
		
		public int X
		{
			get
			{
				return x;	
			}
			set
			{
				x = value;	
				Des();
				Init();
			}
		}
		
		public int Y
		{
			get
			{
				return y;	
			}
			set
			{
				y = value;	
				Des();
				Init();
			}
		}
		
		public string Text
		{
			get
			{
				return text;	
			}
			set
			{
				text = value;	
				Des();
				Init();
			}
		}
		
		public int Color
		{
			get
			{
				return col;	
			}
			set
			{
				col = value;	
				Des();
				Init();
			}
		}
		
		public int Background
		{
			get
			{
				return bg;	
			}
			set
			{
				bg = value;	
				Des();
				Init();
			}
		}
		
		public int Width
		{
			get
			{
				return width;	
			}
			set
			{
				width = value;	
				Des();
				Init();
			}
		}
		
		public int Height
		{
			get
			{
				return height;	
			}
			set
			{
				height = value;	
				Des();
				Init();
			}
		}
		
		public string Name
		{
			get
			{
				return name;	
			}
		}
	}
	
	public class TextBox
	{
		Queen qu;
		int x, y, width, col, bg;
		string text="", name;
		bool isSelect = false;
		
		//Eventi
		public delegate void TextBoxHandler(TextBox sender);
        public event TextBoxHandler TextBoxChange;
		
		private void inputHandler(int typeEvent, int[] ar) {			
			if(typeEvent==1 && isSelect==true)
			{
				char ch = (char)ar[0];
				
				if(ar[0]==127)
				{
					if(text.Length>0)text = text.Substring(0,text.Length-1);
				}
				else if(text.Length<(width/8)) text += ch;
				
				//TextBoxChange(this);
				
				Init();
			}
			
			if(typeEvent==2)
			{			
				int area = qu.areaGet(ar[0],ar[1]);
				if(area != -1)
				{
					if(qu.areaList[area].name == name) isSelect = true;
					else isSelect = false;

					Init();
				}
			}
		}
		
		void Init()
		{
			qu.areaDel(name);
			name = "tex."+ x + "." +y;
			qu.areaAdd(name,x,y,x+width,y+16);
			
			qu.drawRect(x,y,x+width,y+16,bg);
			qu.drawEmptyRect(x,y,x+width,y+16,7);
			qu.qrWrite(x,y+4,text,col,bg);
			if(isSelect==true)qu.qrWrite(x+(text.Length*8),y+4," ",bg,col);
		}
		
		void Des()
		{
			qu.areaDel(name);
			qu.drawRect(x,y,x+width,y+16,qu.Background);
		}
		
		~ TextBox()
		{
		    Des();
		}
		
		public TextBox(Queen qun, int xn, int yn, int widthn)
		{
			qu = qun;
			x = xn;
			y = yn;
			width = widthn;
			
			//Previsione
			col = qu.Background;
			bg = 15 - col;
			text = "";
			
			qun.InputEvent+=new Queen.InputHandler(inputHandler); 
			
			Init();
		}
		
		public bool Selected
		{
			get
			{
				return isSelect;	
			}
			set
			{
				isSelect = value;	
				Init();
			}		
		}
		
		public int X
		{
			get
			{
				return x;	
			}
			set
			{
				x = value;	
				Des();
				Init();
			}
		}
		
		public int Y
		{
			get
			{
				return y;	
			}
			set
			{
				y = value;	
				Des();
				Init();
			}
		}
		
		public string Text
		{
			get
			{
				return text;	
			}
			set
			{
				text = value;	
				Des();
				Init();
			}
		}
		
		public int Color
		{
			get
			{
				return col;	
			}
			set
			{
				col = value;	
				Des();
				Init();
			}
		}
		
		public int Background
		{
			get
			{
				return bg;	
			}
			set
			{
				bg = value;	
				Des();
				Init();
			}
		}
		
		public int Width
		{
			get
			{
				return width;	
			}
			set
			{
				width = value;	
				Des();
				Init();
			}
		}
		
		public string Name
		{
			get
			{
				return name;	
			}
		}
	}	
	
	public class Label
	{
		Queen qu;
		int x, y, width, height, col, bg;
		string text, name;
		
		//Eventi
		public delegate void LabelHandler(Label sender,int typeEvent,int modeEvent);
        public event LabelHandler LabelClick;
		
		private void inputHandler(int typeEvent, int[] ar) {
			if(typeEvent==2)
			{
				int area = qu.areaGet(ar[0],ar[1]);
				if(area != -1)if(qu.areaList[area].name == name); //LabelClick(this,ar[3],ar[4]);
			}
		}
		
		void Init()
		{
			name = "lab."+ x + "." +y;
			qu.areaAdd(name,x,y,x+width,y+height);
			
			int wd=0, hg=0;
			string buf="";
			char[] textList = text.ToCharArray();
			foreach(char c in textList)
			{
				buf += c;
				
				wd+= 8;
				if(wd>width)
				{
					qu.qrWrite(x,y,buf,col,bg);
					hg+=8;
					if(hg>height) break;
				}	
			}
				
		}
		
		void Des()
		{
			qu.areaDel(name);
			qu.drawRect(x,y,x+width,y+height,qu.Background);
		}
		
		~ Label()
		{
		    Des();
		}
		
		public Label(Queen qun, string textn, int xn, int yn)
		{
			qu = qun;
			text = textn;
			x = xn;
			y = yn;
			
			//Previsione
			height = 8;
			width = (text.Length*8);
			col = qu.Background;
			bg = 15 - col;
			
			qun.InputEvent+=new Queen.InputHandler(inputHandler); 
			
			Init();
		}
		
		public int X
		{
			get
			{
				return x;	
			}
			set
			{
				x = value;	
				Des();
				Init();
			}
		}
		
		public int Y
		{
			get
			{
				return y;	
			}
			set
			{
				y = value;	
				Des();
				Init();
			}
		}
		
		public string Text
		{
			get
			{
				return text;	
			}
			set
			{
				text = value;	
				Des();
				Init();
			}
		}
		
		public int Color
		{
			get
			{
				return col;	
			}
			set
			{
				col = value;	
				Des();
				Init();
			}
		}
		
		public int Background
		{
			get
			{
				return bg;	
			}
			set
			{
				bg = value;	
				Des();
				Init();
			}
		}
		
		public int Width
		{
			get
			{
				return width;	
			}
			set
			{
				width = value;	
				Des();
				Init();
			}
		}
		
		public int Height
		{
			get
			{
				return height;	
			}
			set
			{
				height = value;	
				Des();
				Init();
			}
		}
		
		public string Name
		{
			get
			{
				return name;	
			}
		}
	}
}



