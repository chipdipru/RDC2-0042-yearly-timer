/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace RDC2_0041
{
    /// <summary>
    /// Interaction logic for TimeSetWindow.xaml
    /// </summary>
    public partial class TimeSetWindow : Window
    {
        private static readonly string TimeZoneNotSetString = "не установлен";

        public ObservableCollection<string> Days { get; }
        private TimeSettings TimeSet = new TimeSettings();
        private ReadOnlyCollection<TimeZoneInfo> SystemTimeZones = TimeZoneInfo.GetSystemTimeZones();

        public TimeSetWindow(TimeSettings TimeSet)
        {
            InitializeComponent();
            DataContext = this;

            Days = new ObservableCollection<string>(MainWindow.WeekDaysStrings);
            TimeZonesListBox.ItemsSource = SystemTimeZones;            
            TimeZonesListBox.SelectedIndex = 0;

            for (int TimeZoneItemIndex = 0; TimeZoneItemIndex < SystemTimeZones.Count; TimeZoneItemIndex++)            
            {
                if (SystemTimeZones[TimeZoneItemIndex].Id == TimeZoneInfo.Local.Id)
                {
                    TimeZonesListBox.SelectedIndex = TimeZoneItemIndex;
                    break;
                }
            }
            
            this.TimeSet = TimeSet;

            if (TimeSet.Hours < 10)
                TimeTextBlock.Text += "0";
            TimeTextBlock.Text += TimeSet.Hours.ToString() + ":";
            if (TimeSet.Minutes < 10)
                TimeTextBlock.Text += "0";
            TimeTextBlock.Text += TimeSet.Minutes.ToString() + ":";
            if (TimeSet.Seconds < 10)
                TimeTextBlock.Text += "0";
            TimeTextBlock.Text += TimeSet.Seconds.ToString();

            if (TimeSet.Date < 10)
                DateTextBlock.Text += "0";
            DateTextBlock.Text += TimeSet.Date.ToString() + ".";
            if (TimeSet.Month < 10)
                DateTextBlock.Text += "0";
            DateTextBlock.Text += TimeSet.Month.ToString() + ".";
            DateTextBlock.Text += (TimeSet.Year + 2000).ToString();

            DayTextBlock.Text += MainWindow.WeekDaysStrings[TimeSet.WeekDay - 1];

            if (TimeSet.TimeZoneId >= SystemTimeZones.Count)
                TimeZoneTextBlock.Text += TimeZoneNotSetString;
            else
            {
                TimeZoneTextBlock.Text += "\n";
                TimeZoneTextBlock.Text += SystemTimeZones[TimeSet.TimeZoneId].ToString();
            }
        }

        private void acceptButton_Click(object sender, RoutedEventArgs e)
        {
            DateTime CurTime = TimeZoneInfo.ConvertTimeBySystemTimeZoneId(DateTime.UtcNow, SystemTimeZones[TimeZonesListBox.SelectedIndex].Id);
            byte WeekDay = (byte)CurTime.DayOfWeek;

            if (WeekDay != 0)
                WeekDay--;
            else
                WeekDay = 6;

            WeekDay++; //дни c 1 по 7

            TimeSet.Seconds = DecToBin((byte)CurTime.Second);
            TimeSet.Minutes = DecToBin((byte)CurTime.Minute);
            TimeSet.Hours = DecToBin((byte)CurTime.Hour);
            TimeSet.WeekDay = WeekDay;
            TimeSet.Date = DecToBin((byte)CurTime.Day);
            TimeSet.Month = DecToBin((byte)CurTime.Month);
            TimeSet.Year = DecToBin((byte)(CurTime.Year - 2000));
            TimeSet.TimeZoneId = (byte)TimeZonesListBox.SelectedIndex;

            this.DialogResult = true;
        }
        
        private byte DecToBin (byte InVal)
        {
            byte Tens = (byte)(InVal / 10);
            byte NewValue = (byte)(Tens << 4);
            NewValue |= (byte)(InVal - Tens * 10);
            
            return NewValue;
        }

        private void SyncWithPCCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            AcceptButton.IsEnabled = true;
        }

        private void SyncWithPCCheckBox_UnChecked(object sender, RoutedEventArgs e)
        {
            AcceptButton.IsEnabled = false;
        }
    }
}
