﻿using ExtendedSerialPort_NS;
using System.IO.Ports;
using System.Net.NetworkInformation;
using System.Text;
using System.Windows;
using System.Windows.Controls;
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
            serialPort1 = new ExtendedSerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick; ;
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object? sender, EventArgs e)
        {
            for (int i = 0; i < robot.byteListReceived.Count; i++)
            {
                DecodeMessage(robot.byteListReceived.Dequeue());
            }
        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)

        {
            for (int i = 0; i < e.Data.Length; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);
            }
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
            //string messageStr = "Bonjour";
            //byte[] msgPayload = Encoding.ASCII.GetBytes(messageStr);
            //int msgPayloadLength = msgPayload.Length;
            //int msgFunction = 0x0080;
            //UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);
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
        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }
        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;
        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                        rcvState = StateReception.FunctionMSB;
                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction |= c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength |= c;

                    if (msgDecodedPayloadLength > 1024)
                    {
                        rcvState = StateReception.Waiting;
                    }
                    else if (msgDecodedPayloadLength > 0)
                    {
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                        msgDecodedPayloadIndex = 0;
                        rcvState = StateReception.Payload;
                    }
                    else
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    break;
                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex++] = c;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    break;
                case StateReception.CheckSum:
                    byte receivedChecksum = c;
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

                    if (receivedChecksum == calculatedChecksum)
                    {

                        ProcessDecodeMessage(msgDecodedFunction, msgDecodedPayload);
                    }
                    else
                    {
                        textboxReception.Text += "Erreur de checksum ! Message rejeté.\n";
                    }
                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

        bool ELVerte = false;
        bool ELRouge = false;
        bool ELOrange = false;
        bool ELBleue = false;
        bool ELBlanche = false;

        private void LEDRouge(object sender, RoutedEventArgs e)
        {
            ELRouge = !ELRouge;
            byte[] array = new byte [2];
            array [0] = 0;
            array [1] = Convert.ToByte(ELRouge);
            UartEncodeAndSendMessage(0x0020,2, array);
        }

        private void LEDOrange(object sender, RoutedEventArgs e)
        {
            ELOrange = !ELOrange;
            byte[] array = new byte[2];
            array[0] = 1;
            array[1] = Convert.ToByte(ELOrange);
            UartEncodeAndSendMessage(0x0020, 2, array);
        }

        private void LEDBlanche(object sender, RoutedEventArgs e)
        {
            ELBlanche = !ELBlanche;
            byte[] array = new byte[2];
            array[0] = 2;
            array[1] = Convert.ToByte(ELBlanche);
            UartEncodeAndSendMessage(0x0020, 2, array);
        }

        private void LEDBleue(object sender, RoutedEventArgs e)
        {
            ELBleue = !ELBleue;
            byte[] array = new byte[2];
            array[0] = 3;
            array[1] = Convert.ToByte(ELBleue);
            UartEncodeAndSendMessage(0x0020, 2, array);
        }

        private void LEDVerte(object sender, RoutedEventArgs e)
        {
            ELVerte = !ELVerte;
            byte[] array = new byte[2];
            array[0] = 4;
            array[1] = Convert.ToByte(ELVerte);
            UartEncodeAndSendMessage(0x0020, 2, array);
        }

        public enum StateRobot
        {
            STATE_ATTENTE = 0,
            STATE_ATTENTE_EN_COURS = 1,
            STATE_AVANCE = 2,
            STATE_AVANCE_EN_COURS = 3,
            STATE_TOURNE_GAUCHE = 4,
            STATE_TOURNE_GAUCHE_EN_COURS = 5,
            STATE_TOURNE_DROITE = 6,
            STATE_TOURNE_DROITE_EN_COURS = 7,
            STATE_TOURNE_SUR_PLACE_GAUCHE = 8,
            STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS = 9,
            STATE_TOURNE_SUR_PLACE_DROITE = 10,
            STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS = 11,
            STATE_ARRET = 12,
            STATE_ARRET_EN_COURS = 13,
            STATE_RECULE = 14,
            STATE_RECULE_EN_COURS = 15
        }

        byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            ;
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

        private void ProcessDecodeMessage(int msgFunction, byte[] msgPayload)
        {
            switch (msgFunction)
            {
                case 0x0020:
                    int Led = (int)msgPayload[0];
                    if (Led == 0) 
                    {
                        ELVerte = Convert.ToBoolean(msgPayload[1]);
                    }
                    else if (Led == 1)
                    {
                        ELBleue = Convert.ToBoolean(msgPayload[1]);
                    }
                    else if (Led == 2)
                    {
                        ELBlanche = Convert.ToBoolean(msgPayload[1]);
                    }
                    else if (Led == 3)
                    {
                        ELOrange = Convert.ToBoolean(msgPayload[1]);
                    }
                    else if (Led == 4)
                    {
                        ELRouge = Convert.ToBoolean(msgPayload[1]);
                    }
                    break;
                /*case 0x0030:
                    byte[] value = new byte[2];
                    Buffer.BlockCopy(msgPayload, 0, value, 0, 2);
                    ValueIRExGauche.Content = BitConverter.ToInt16(value, 0);
                    Buffer.BlockCopy(msgPayload, 2, value, 0, 2);
                    ValueIRGauche.Content = BitConverter.ToInt16(value, 0);
                    Buffer.BlockCopy(msgPayload, 4, value, 0, 2);
                    ValueIRCentre.Content = BitConverter.ToInt16(value, 0);
                    Buffer.BlockCopy(msgPayload, 6, value, 0, 2);
                    ValueIRDroit.Content = BitConverter.ToInt16(value, 0);
                    Buffer.BlockCopy(msgPayload, 8, value, 0, 2);
                    ValueIRExDroit.Content = BitConverter.ToInt16(value, 0);
                    break;

                case 0x0040:

                    break;

                case 0x0050:
                    break;

                case 0x0051:
                    break;

                case MsgFunction.RobotState:
                    int instant = (((int)msgPayload[1]) << 24) + (((int)msgPayload[2]) << 16)
                    + (((int)msgPayload[3]) << 8) + ((int)msgPayload[4]);
                    rtbReception.Text += "\nRobot␣State␣:␣" +
                    ((StateRobot)(msgPayload[0])).ToString() +
                    "␣-␣" + instant.ToString() + "␣ms";
                    break;
                */
                default:
                    break;
            }
        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            string messageStr = "Bonjour";
            byte[] msgPayload = Encoding.ASCII.GetBytes(messageStr);
            int msgPayloadLength = msgPayload.Length;
            int msgFunction = 0x0080;
            UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);
        }

        private void CheckBox_Checked_1(object sender, RoutedEventArgs e)
        {

        }

        private void CheckBox_Checked_2(object sender, RoutedEventArgs e)
        {

        }

        private void CheckBox_Checked_3(object sender, RoutedEventArgs e)
        {

        }

        private void CheckBox_Unchecked(object sender, RoutedEventArgs e)
        {

        }

        private void Mot_KeyUp(object sender, KeyEventArgs e)
        {

        }
    }
}

