using ExtendedSerialPort_NS;
using System.IO.Ports;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Threading;
namespace RobotInterface

{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Robot robot = new Robot();
        ExtendedSerialPort serialPort1;

        DispatcherTimer timerAffichage;
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ExtendedSerialPort("COM7", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick; ;
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object? sender, EventArgs e)
        {
            //textboxReception.Text += robot.receivedText;
            //robot.receivedText = "";
            for (int i = 0; i < robot.byteListReceived.Count; i++)
            {
                textboxReception.Text += "0x"+robot.byteListReceived.Dequeue().ToString("X2") + " ";
            }
        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)

        {
            for (int i = 0; i < e.Data.Length; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);

            }
            //string receivedText = Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            //this.robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
        }


        private void buttonSend_Click(object sender, RoutedEventArgs e)
        {

        }

        private void ButtonSend_Click(object sender, RoutedEventArgs e)
        {
            buttonSend.Background = Brushes.Beige;
            SendMessage();
        }

        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }


        private void SendMessage()
        {
            serialPort1.WriteLine(textboxEmission.Text);
            textboxEmission.Text = "";
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            textboxReception.Text = "";
        }

        private void ButtonTest_Click(object sender, RoutedEventArgs e)
        {
            /*byte[] byteList = new byte[20];
            for (int i = 0; i < byteList.Length; i++)
            {
                byteList[i] = (byte)(2 * i);
            }
            byteList[byteList.Length - 1] = (byte)'\n';
            serialPort1.Write(byteList, 0, byteList.Length);*/
            string messageStr = "Bonjour";
            byte[] msgPayload = Encoding.ASCII.GetBytes(messageStr);
            int msgPayloadLength = msgPayload.Length;
            int msgFunction = 0x0080;
            UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);
        }

        byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte c = 0;
            c ^= 0xFE;
            c ^= (byte)(msgFunction >> 8);
            c ^= (byte)(msgFunction);
            c ^= (byte)(msgPayloadLength >> 8);
            c ^= (byte)(msgPayloadLength);
            foreach (byte b in msgPayload)
            {
                c ^= b;
            }
            return c;
        }

        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] message = new byte[6 + msgPayloadLength];
            int pos = 0;
            message[pos++] = 0xFE;
            message[pos++] = (byte)(msgFunction >> 8);
            message[pos++] = (byte)(msgFunction);
            message[pos++] = (byte)(msgPayloadLength >> 8);
            message[pos++] = (byte)(msgPayloadLength);
            for (int i = 0; i < msgPayloadLength; i++)
            {
                message[pos++] = msgPayload[i];
            }
            byte c = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
            message[pos++] = c;
            serialPort1.Write(message, 0, pos);
        }
    }
}
