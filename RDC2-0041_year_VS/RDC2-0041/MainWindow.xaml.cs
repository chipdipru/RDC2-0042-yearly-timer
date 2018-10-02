/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/

using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace RDC2_0041
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public const byte DAYS_NUM = 7;
        private const byte EVENTS_MAX = 32;
        public const byte TEMP_EVENTS_MAX = 8;

        private const Int32 USB_PACKET_SIZE = 64;

        private const byte USB_REPORT_ID = 1;
        
        private const byte USB_CMD_POS = 1;
        
        private const byte USB_CMD_EEPROM_WRITE = 0;
        private const byte USB_CMD_EEPROM_READ = 1;
        private const byte USB_CMD_TIME = 2;

        private const byte EEPROM_SUBCMD_EVENTS_COUNT = 0xC0;
        private const byte EEPROM_SUBCMD_EVENTS_DATA = 0xC1;        
        private const byte TIME_SUBCMD_SET = 0xC5;
        private const byte TIME_SUBCMD_GET = 0xC6;

        private const byte USB_EEPROM_SUBCMD_POS = 2;
        private const byte USB_ACTIVE_EVENT_POS = USB_EEPROM_SUBCMD_POS + 1;
        private const byte USB_EVENTS_IN_PACKET_POS = USB_ACTIVE_EVENT_POS + 2;
        private const byte USB_EVENTS_DATA_POS = USB_EVENTS_IN_PACKET_POS + 1;

        private const byte USB_EVENT_HOUR_OFFSET = 0;
        private const byte USB_EVENT_MINUTE_OFFSET = 1;
        private const byte USB_EVENT_STATE_OFFSET = 2;
        private const byte USB_EVENT_MONTH_OFFSET = 3;
        private const byte USB_EVENT_DAY_OFFSET = 4;
        private const byte USB_EVENT_SIZE = 5;

        private const byte USB_TIME_SUBCMD_POS = USB_EEPROM_SUBCMD_POS;
        private const byte USB_TIME_SECONDS_POS = (USB_TIME_SUBCMD_POS + 1);
        private const byte USB_TIME_MINUTES_POS = (USB_TIME_SECONDS_POS + 1);
        private const byte USB_TIME_HOURS_POS = (USB_TIME_MINUTES_POS + 1);
        private const byte USB_TIME_WEEKDAY_POS = (USB_TIME_HOURS_POS + 1);
        private const byte USB_TIME_DATE_POS = (USB_TIME_WEEKDAY_POS + 1);
        private const byte USB_TIME_MONTH_POS = (USB_TIME_DATE_POS + 1);
        private const byte USB_TIME_YEAR_POS = (USB_TIME_MONTH_POS + 1);
        private const byte USB_TIME_ZONE_POS = (USB_TIME_YEAR_POS + 1);

        private const byte USB_START_STOP_FLAG_POS = 63;
        private const byte USB_CMD_STOP_FLAG = 0xA0;
        private const byte USB_CMD_START_FLAG = 0x55;

        private const byte USB_EVENT_IN_PACKET_MAX = 6;        
        private const byte USB_START_HOUR_OFFSET = 9;       
        private const byte THREAD_SET_EVENTS = 0;
        

        public static readonly string[] WeekDaysStrings =
        {
            "ПОНЕДЕЛЬНИК",
            "ВТОРНИК",
            "СРЕДА",
            "ЧЕТВЕРГ",
            "ПЯТНИЦА",
            "СУББОТА",
            "ВОСКРЕСЕНЬЕ",
        };

        public static readonly string[] MonthNames =
        {
          "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
          "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь",
        };

        private static readonly string DeviceConnectedString = "Таймер RDC2-0042 подключен.";
        private static readonly string DeviceNotConnectedString = "Таймер RDC2-0042 не подключен. Подключите устройство и перезапустите программу.";

        private const byte MonthDays_31 = 31;
        private const byte MonthDays_30 = 30;
        private const byte MonthDays_29 = 29;
        private const byte MonthsCount = 12;
        private const int EVENTS_COUNT_MAX = 1524;

        private const byte MENU_MONTH_COPY_INDEX = 0;
        private const byte MENU_MONTH_PASTE_INDEX = 1;
        private const byte MENU_MONTH_DELETE_INDEX = 2;

        private const byte MENU_EVENT_NEW_INDEX = 0;
        private const byte MENU_EVENT_COPY_INDEX = 1;
        private const byte MENU_EVENT_PASTE_INDEX = 2;
        private const byte MENU_EVENT_DELETE_INDEX = 3;


        private ObservableCollection<Month> Months = new ObservableCollection<Month>();
        private ObservableCollection<Event> EventsCopyBuffer = new ObservableCollection<Event>();
        private Month MonthCopyBuffer = null;

        private DispatcherTimer ThreadTimer = new DispatcherTimer();
        private ProgressWindow ProgressBar = new ProgressWindow();
        private bool ThreadWrite = false;
        private byte ThreadMonth = 0;
        private byte ThreadDay = 0;
        private byte ThreadEventInDay = 0;
        private int ThreadEvent = 0;
        private int TotalEventsCount = 0;

               
        public MainWindow()
        {
            InitializeComponent();
            
            byte[] MonthDaysCount = {
                                        31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
                                    };


            for (byte month = 0; month < MonthsCount; month++)            
                Months.Add(new Month(MonthNames[month], MonthDaysCount[month]));
                        
            EventsList.ItemsSource = Months[0].Days[0].Events;
            MonthDays.ItemsSource = Months[0].Days;
            MonthDays.SelectedItem = Months[0].Days[0];
            MonthName.Text = Months[0].Name;
            MonthsList.ItemsSource = Months;
            EventsCountUpdate();

            Boolean USBDevDetected = USB_device.Open();
            
            if (USBDevDetected)
            {                                
                ThreadTimer.Interval = TimeSpan.FromMilliseconds(10); //период опроса устройства
                ThreadTimer.Tick += ThreadTimer_Tick;
            }

            ShowConnectionState(USBDevDetected);
        }

        private void NewEventButton_Click(object sender, RoutedEventArgs e)
        {
            Event EventToAdd = new Event();
            EventWindow NewEventWindow = new EventWindow(EventToAdd);
            NewEventWindow.Owner = this;
            NewEventWindow.ShowDialog();
            
            if (EventToAdd.IsActive == true)
            {
                EventsCopyBuffer.Clear();
                EventsCopyBuffer.Add(new Event() { Hours = EventToAdd.Hours, Minutes = EventToAdd.Minutes, RelayState = EventToAdd.RelayState, IsActive = true });
                PasteEvent((MonthDays.SelectedItem as MonthDay).Events);
                EventsCopyBuffer.Clear();

                EventsCountUpdate();
            }
        }
        
        private void DeleteEventButton_Click(object sender, RoutedEventArgs e)
        {
            if ((EventsList.IsKeyboardFocusWithin == true) && (EventsList.SelectedItems.Count != 0))
            {
                ObservableCollection<Event> DelBuffer = new ObservableCollection<Event>();

                foreach (Event ItemToCopy in EventsList.SelectedItems)
                    DelBuffer.Add(ItemToCopy);
                
                foreach (Event ItemToDel in DelBuffer)
                    (MonthDays.SelectedItem as MonthDay).Events.Remove(ItemToDel);
            }

            else if ((MonthDays.IsKeyboardFocusWithin == true) && (MonthDays.SelectedItems.Count != 0))
            {
                foreach (MonthDay DayItem in MonthDays.SelectedItems)
                    DayItem.Events.Clear();
            }

            EventsCountUpdate();
        }
                
        private void CopyEventButton_Click(object sender, RoutedEventArgs e)
        {            
            if ((EventsList.IsKeyboardFocusWithin == true) && (EventsList.SelectedItems.Count != 0))
            {
                EventsCopyBuffer.Clear();

                foreach (Event ItemToCopy in EventsList.SelectedItems)
                    EventsCopyBuffer.Add(new Event() { Hours = ItemToCopy.Hours, Minutes = ItemToCopy.Minutes, RelayState = ItemToCopy.RelayState, IsActive = true });
            }

            else if ((MonthDays.IsKeyboardFocusWithin) && (MonthDays.SelectedItem != null))
            {
                EventsCopyBuffer.Clear();

                foreach (Event ItemToCopy in (MonthDays.SelectedItem as MonthDay).Events)
                    EventsCopyBuffer.Add(new Event() { Hours = ItemToCopy.Hours, Minutes = ItemToCopy.Minutes, RelayState = ItemToCopy.RelayState, IsActive = true });
            }
        }

        private void PasteEventButton_Click(object sender, RoutedEventArgs e)
        {
            foreach (MonthDay DayItem in MonthDays.SelectedItems)
                PasteEvent(DayItem.Events);

            EventsCountUpdate();
        }

        private void EditEventButton_Click(object sender, RoutedEventArgs e)
        {
            if (EventsList.SelectedItems.Count == 1)
            {                
                Event EventToEdit = new Event();
                Event EventLastSet = new Event();
                EventToEdit.Hours = (MonthDays.SelectedItem as MonthDay).Events[EventsList.SelectedIndex].Hours;
                EventToEdit.Minutes = (MonthDays.SelectedItem as MonthDay).Events[EventsList.SelectedIndex].Minutes;
                EventToEdit.RelayState = (MonthDays.SelectedItem as MonthDay).Events[EventsList.SelectedIndex].RelayState;
                EventToEdit.IsActive = true;
                EventLastSet.Hours = (MonthDays.SelectedItem as MonthDay).Events[EventsList.SelectedIndex].Hours;
                EventLastSet.Minutes = (MonthDays.SelectedItem as MonthDay).Events[EventsList.SelectedIndex].Minutes;
                EventLastSet.RelayState = (MonthDays.SelectedItem as MonthDay).Events[EventsList.SelectedIndex].RelayState;
                EventLastSet.IsActive = true;
                EventWindow NewEventWindow = new EventWindow(EventToEdit);
                NewEventWindow.Owner = this;
                NewEventWindow.ShowDialog();

                if (EventToEdit.IsActive == true)
                {
                    if (((MonthDays.SelectedItem as MonthDay).Events[EventsList.SelectedIndex].Hours == EventToEdit.Hours)
                     && ((MonthDays.SelectedItem as MonthDay).Events[EventsList.SelectedIndex].Minutes == EventToEdit.Minutes))
                    {
                        (MonthDays.SelectedItem as MonthDay).Events[EventsList.SelectedIndex].RelayState = EventToEdit.RelayState;
                    }

                    else
                    {
                        int NewItemIndex = -1;
                        int ItemIndex = EventsList.SelectedIndex;

                        (MonthDays.SelectedItem as MonthDay).Events.RemoveAt(EventsList.SelectedIndex);

                        foreach (Event EventToComp in (MonthDays.SelectedItem as MonthDay).Events)
                        {
                            if ((EventToEdit.Hours < EventToComp.Hours)
                             || ((EventToEdit.Hours == EventToComp.Hours) && (EventToEdit.Minutes < EventToComp.Minutes)))
                            {
                                NewItemIndex = (MonthDays.SelectedItem as MonthDay).Events.IndexOf(EventToComp);
                                break;
                            }

                            else if ((EventToEdit.Hours == EventToComp.Hours) && (EventToEdit.Minutes == EventToComp.Minutes))
                            {
                                NewItemIndex = EVENTS_MAX;
                                break;
                            }
                        }

                        if ((NewItemIndex >= 0) && (NewItemIndex < EVENTS_MAX))
                            (MonthDays.SelectedItem as MonthDay).Events.Insert(NewItemIndex, new Event() { Hours = EventToEdit.Hours, Minutes = EventToEdit.Minutes, RelayState = EventToEdit.RelayState, IsActive = true });

                        else if (NewItemIndex == EVENTS_MAX)
                        {
                            EventsCopyBuffer.Clear();
                            EventsCopyBuffer.Add(new Event() { Hours = EventToEdit.Hours, Minutes = EventToEdit.Minutes, RelayState = EventToEdit.RelayState, IsActive = true });
                            if (PasteEvent((MonthDays.SelectedItem as MonthDay).Events) == -1)
                                (MonthDays.SelectedItem as MonthDay).Events.Insert(ItemIndex, new Event() { Hours = EventLastSet.Hours, Minutes = EventLastSet.Minutes, RelayState = EventLastSet.RelayState, IsActive = true });
                            EventsCopyBuffer.Clear();
                        }
                            
                        else
                            (MonthDays.SelectedItem as MonthDay).Events.Add(new Event() { Hours = EventToEdit.Hours, Minutes = EventToEdit.Minutes, RelayState = EventToEdit.RelayState, IsActive = true });
                    }
                }

                EventsCountUpdate();
            }
        }

        private void OpenFileButton_Click(object sender, ExecutedRoutedEventArgs e)
        {            
            if (EventsWarningResult() == true)
            {
                OpenFileDialog ChooseFile = new OpenFileDialog();
                ChooseFile.Filter = "Text files (*.txt)|*.txt";
                if (ChooseFile.ShowDialog() == true)
                {
                    OpenFile(ChooseFile.FileName);
                }
            }
        }

        private void SaveFileButton_Click(object sender, ExecutedRoutedEventArgs e)
        {
            SaveFileDialog ChooseFile = new SaveFileDialog();
            ChooseFile.Filter = "Text files (*.txt)|*.txt";
            if (ChooseFile.ShowDialog() == true)
            {
                using (StreamWriter FileToWriteTo = File.CreateText(ChooseFile.FileName))
                {
                    foreach (Month MonthItem in Months)
                    {
                        FileToWriteTo.WriteLine("---------{0}", MonthItem.Name);

                        foreach (MonthDay DayItem in MonthItem.Days)
                        {
                            if (DayItem.Events.Count != 0)
                            {
                                FileToWriteTo.WriteLine("----------------{0} день", DayItem.Number);

                                foreach (Event EventItem in DayItem.Events)
                                {
                                    FileToWriteTo.WriteLine("{0}:{1} Реле: {2};", EventItem.Hours, EventItem.Minutes, EventItem.RelayState);
                                }
                            }                            
                        }
                    }                      
                }
            }
        }
        
        private void DownloadButton_Click(object sender, RoutedEventArgs e)
        {
            DownloadStart();            
        }

        private void UpLoadButton_Click(object sender, RoutedEventArgs e)
        {            
            if (EventsWarningResult() == true)
                ReadSettings();
        }
        
        private void ThreadTimer_Tick(object sender, EventArgs e)
        {
            if (ThreadWrite)//запись
            {
                byte[] USBPacket = new byte[USB_PACKET_SIZE];
                USBPacket[0] = USB_REPORT_ID;
                USBPacket[USB_CMD_POS] = USB_CMD_EEPROM_WRITE;
                USBPacket[USB_EEPROM_SUBCMD_POS] = EEPROM_SUBCMD_EVENTS_DATA;
                USBPacket[USB_ACTIVE_EVENT_POS] = (byte)ThreadEvent;
                USBPacket[USB_ACTIVE_EVENT_POS + 1] = (byte)(ThreadEvent >> 8);
                USBPacket[USB_EVENTS_IN_PACKET_POS] = 0; //событий в посылке
                
                for (byte EventInPacket = 0; EventInPacket < USB_EVENT_IN_PACKET_MAX; EventInPacket++)
                {
                    USBPacket[USB_EVENTS_IN_PACKET_POS]++;

                    int EventStartPos = USB_EVENTS_DATA_POS + EventInPacket * USB_EVENT_SIZE;
                    USBPacket[EventStartPos + USB_EVENT_HOUR_OFFSET] = Months[ThreadMonth].Days[ThreadDay].Events[ThreadEventInDay].Hours;
                    USBPacket[EventStartPos + USB_EVENT_MINUTE_OFFSET] = Months[ThreadMonth].Days[ThreadDay].Events[ThreadEventInDay].Minutes;
                    USBPacket[EventStartPos + USB_EVENT_STATE_OFFSET] = (byte)Array.IndexOf(EventWindow.RelayStatesStrings, Months[ThreadMonth].Days[ThreadDay].Events[ThreadEventInDay].RelayState);
                    USBPacket[EventStartPos + USB_EVENT_MONTH_OFFSET] = (byte)(ThreadMonth + 1);
                    USBPacket[EventStartPos + USB_EVENT_DAY_OFFSET] = Months[ThreadMonth].Days[ThreadDay].Number;

                    ThreadEvent++;
                    if ((ThreadEvent < TotalEventsCount) && (ThreadEvent < EVENTS_COUNT_MAX))
                    {
                        ThreadEventInDay++;
                        FindNextEvent();
                    }
                        
                    else
                    {
                        ThreadTimer.Stop();
                        ProgressBar.Hide();

                        USBPacket[USB_START_STOP_FLAG_POS] = USB_CMD_STOP_FLAG;

                        break;
                    }
                }
                
                Boolean USBSuccess = USB_device.Write(USBPacket);
            }

            else //чтение
            {
                byte[] USBPacket = new byte[USB_PACKET_SIZE];
                USBPacket[0] = USB_REPORT_ID;
                USBPacket[USB_CMD_POS] = USB_CMD_EEPROM_READ;
                USBPacket[USB_EEPROM_SUBCMD_POS] = EEPROM_SUBCMD_EVENTS_DATA;
                USBPacket[USB_ACTIVE_EVENT_POS] = (byte)ThreadEvent;
                USBPacket[USB_ACTIVE_EVENT_POS + 1] = (byte)(ThreadEvent >> 8);

                int EventsCount = TotalEventsCount - ThreadEvent;
                if (EventsCount > USB_EVENT_IN_PACKET_MAX)
                    EventsCount = USB_EVENT_IN_PACKET_MAX;

                USBPacket[USB_EVENTS_IN_PACKET_POS] = (byte)EventsCount;

                Boolean USBSuccess = USB_device.Write(USBPacket);
                USBSuccess = USB_device.Read(USBPacket);

                for (byte EventInPacket = 0; EventInPacket < EventsCount; EventInPacket++)
                {
                    int EventStartPos = USB_EVENTS_DATA_POS + EventInPacket * USB_EVENT_SIZE;
                    byte ActiveMonth = (byte)(USBPacket[EventStartPos + USB_EVENT_MONTH_OFFSET] - 1);
                    byte ActiveDay = (byte)(USBPacket[EventStartPos + USB_EVENT_DAY_OFFSET] - 1);

                    Months[ActiveMonth].Days[ActiveDay].Events.Add(new Event()
                    {
                        Hours = USBPacket[EventStartPos + USB_EVENT_HOUR_OFFSET],
                        Minutes = USBPacket[EventStartPos + USB_EVENT_MINUTE_OFFSET],
                        RelayState = EventWindow.RelayStatesStrings[USBPacket[EventStartPos + USB_EVENT_STATE_OFFSET]],
                    });
                }

                ThreadEvent += USBPacket[USB_EVENTS_IN_PACKET_POS];
                if (ThreadEvent >= TotalEventsCount)
                {
                    ThreadTimer.Stop();
                    ProgressBar.Hide();
                    EventsCountUpdate();
                    MonthEventsCountUpdate();
                }
            }
        }

        private void ReadSettings()
        {            
            ThreadEvent = 0;
            DeleteAllEvents();

            byte[] USBPacket = new byte[USB_PACKET_SIZE];
            USBPacket[0] = USB_REPORT_ID;
            USBPacket[USB_CMD_POS] = USB_CMD_EEPROM_READ;
            USBPacket[USB_EEPROM_SUBCMD_POS] = EEPROM_SUBCMD_EVENTS_COUNT;
            Boolean USBSuccess = USB_device.Write(USBPacket);

            ShowConnectionState(USBSuccess);
            if (USBSuccess == false)
                return;

            USBSuccess = USB_device.Read(USBPacket);
            
            TotalEventsCount = (USBPacket[USB_ACTIVE_EVENT_POS + 1] << 8) | USBPacket[USB_ACTIVE_EVENT_POS];
            
            if (TotalEventsCount > 0)
            {
                ProgressBar.Owner = this;
                ProgressBar.Show();
                ThreadWrite = false;
                ThreadTimer.Start();
            }            
        }
        
        private int PasteEvent(ObservableCollection<Event> DestinationCollection)
        {            
            int SameEventsAction = 0;
            
            foreach (Event ItemToCopy in EventsCopyBuffer)
            {
                bool SameTimeEvents = false;

                foreach (Event EventToComp in DestinationCollection)
                {
                    if ((ItemToCopy.Hours == EventToComp.Hours) && (ItemToCopy.Minutes == EventToComp.Minutes))
                    {
                        SameTimeEvents = true;
                        break;
                    }
                }

                if (SameTimeEvents)
                {
                    bool? WindowRes;
                    PasteActionWindow PasteDialog = new PasteActionWindow();
                    PasteDialog.Owner = this;
                    WindowRes = PasteDialog.ShowDialog();
                    if (WindowRes == true)
                    {
                        if (PasteDialog.Action == PasteActionWindow.Skip)
                        {
                            SameEventsAction = 1;
                        }

                        else if (PasteDialog.Action == PasteActionWindow.Replace)
                        {
                            SameEventsAction = 2;
                        }
                    }
                    else
                        SameEventsAction = -1;

                    break;
                }
            }

            if (SameEventsAction != -1)
            {
                foreach (Event ItemToCopy in EventsCopyBuffer)
                {
                    int NewItemIndex = -1;
                    bool SameTimeEvents = false;

                    foreach (Event EventToComp in DestinationCollection)
                    {
                        if ((ItemToCopy.Hours < EventToComp.Hours)
                         || ((ItemToCopy.Hours == EventToComp.Hours) && (ItemToCopy.Minutes < EventToComp.Minutes))
                         || ((ItemToCopy.Hours == EventToComp.Hours) && (ItemToCopy.Minutes == EventToComp.Minutes)))
                        {
                            NewItemIndex = DestinationCollection.IndexOf(EventToComp);

                            if (((ItemToCopy.Hours == EventToComp.Hours) && (ItemToCopy.Minutes == EventToComp.Minutes)))
                                SameTimeEvents = true;
                            break;
                        }
                    }

                    if ((SameEventsAction == 0) || ((SameEventsAction != 0) && (!SameTimeEvents)))
                    {
                        if (NewItemIndex != -1)
                            DestinationCollection.Insert(NewItemIndex, new Event() { Hours = ItemToCopy.Hours, Minutes = ItemToCopy.Minutes, RelayState = ItemToCopy.RelayState, IsActive = true });
                        else
                            DestinationCollection.Add(new Event() { Hours = ItemToCopy.Hours, Minutes = ItemToCopy.Minutes, RelayState = ItemToCopy.RelayState, IsActive = true });
                    }
                    else if (SameTimeEvents)
                    {
                        if (SameEventsAction == 1)
                        {

                        }
                        else if (SameEventsAction == 2)
                        {
                            if (NewItemIndex != -1)
                            {
                                DestinationCollection.RemoveAt(NewItemIndex);
                                DestinationCollection.Insert(NewItemIndex, new Event() { Hours = ItemToCopy.Hours, Minutes = ItemToCopy.Minutes, RelayState = ItemToCopy.RelayState, IsActive = true });
                            }
                        }
                    }
                }
            }

            return SameEventsAction;
        }

        private void TimeSetButton_Click(object sender, RoutedEventArgs e)
        {
            TimeSettings TimeSet = new TimeSettings();

            byte[] USBPacket = new byte[USB_PACKET_SIZE];
            USBPacket[0] = USB_REPORT_ID;
            USBPacket[USB_CMD_POS] = USB_CMD_TIME;
            USBPacket[USB_TIME_SUBCMD_POS] = TIME_SUBCMD_GET;

            Boolean USBSuccess = USB_device.Write(USBPacket);
            ShowConnectionState(USBSuccess);
            if (USBSuccess == false)
                return;

            USBSuccess = USB_device.Read(USBPacket);
            ShowConnectionState(USBSuccess);
            if (USBSuccess == false)
                return;

            TimeSet.Seconds = USBPacket[USB_TIME_SECONDS_POS];
            TimeSet.Minutes = USBPacket[USB_TIME_MINUTES_POS];
            TimeSet.Hours = USBPacket[USB_TIME_HOURS_POS];
            TimeSet.WeekDay = USBPacket[USB_TIME_WEEKDAY_POS];
            TimeSet.Date = USBPacket[USB_TIME_DATE_POS];
            TimeSet.Month = USBPacket[USB_TIME_MONTH_POS];
            TimeSet.Year = USBPacket[USB_TIME_YEAR_POS];
            TimeSet.TimeZoneId = USBPacket[USB_TIME_ZONE_POS];

            TimeSetWindow TimeSettings = new TimeSetWindow(TimeSet);
            TimeSettings.Owner = this;
            bool? SetResult = TimeSettings.ShowDialog();

            if (SetResult == true)
            {
                USBPacket = new byte[USB_PACKET_SIZE];
                USBPacket[0] = USB_REPORT_ID;
                USBPacket[USB_CMD_POS] = USB_CMD_TIME;
                USBPacket[USB_TIME_SUBCMD_POS] = TIME_SUBCMD_SET;
                USBPacket[USB_TIME_SECONDS_POS] = TimeSet.Seconds;
                USBPacket[USB_TIME_MINUTES_POS] = TimeSet.Minutes;
                USBPacket[USB_TIME_HOURS_POS] = TimeSet.Hours;
                USBPacket[USB_TIME_WEEKDAY_POS] = TimeSet.WeekDay;
                USBPacket[USB_TIME_DATE_POS] = TimeSet.Date;
                USBPacket[USB_TIME_MONTH_POS] = TimeSet.Month;
                USBPacket[USB_TIME_YEAR_POS] = TimeSet.Year;
                USBPacket[USB_TIME_ZONE_POS] = TimeSet.TimeZoneId;

                USBSuccess = USB_device.Write(USBPacket);

                ShowConnectionState(USBSuccess);
                if (USBSuccess == false)
                    return;
            }
        }

        private void OpenFile(string FileToOpen)
        {
            DeleteAllEvents();
            
            using (StreamReader FileToReadFrom = File.OpenText(FileToOpen))
            {
                int DayIndex = 0;
                int MonthIndex = 0;
                bool IsLineAMonthDay;
                bool IsLineAMonth;

                while (FileToReadFrom.Peek() >= 0)
                {
                    StringBuilder NewLine = new StringBuilder(FileToReadFrom.ReadLine());

                    NewLine.Replace(" ", "");

                    IsLineAMonth = false;
                    IsLineAMonthDay = false;

                    for (byte Index = 0; Index < MonthsCount; Index++)
                    {
                        if (NewLine.ToString().IndexOf(Months[Index].Name) != -1)
                        {
                            MonthIndex = Index;
                            IsLineAMonth = true;
                            break;
                        }
                    }

                    if (IsLineAMonth == false)
                    {
                        if (NewLine.ToString().IndexOf("день") != -1)
                        {
                            NewLine.Replace("день", "");
                            NewLine.Replace("-", "");
                            if (int.TryParse(NewLine.ToString(), out DayIndex) == false)
                                continue;

                            DayIndex--;
                            IsLineAMonthDay = true;
                        }
                    }
                    
                    if ((IsLineAMonth == false) && (IsLineAMonthDay == false))
                    {                        
                        Event DayEvent = new Event();
                        DayEvent.IsActive = true;
                        
                        byte NumValue;
                        int StringIndex;

                        StringIndex = NewLine.ToString().IndexOf(":");
                        if (StringIndex == -1)
                            continue;

                        if (byte.TryParse(NewLine.ToString().Substring(0, StringIndex), out NumValue) == false)
                            continue;

                        DayEvent.Hours = NumValue;
                        NewLine.Remove(0, StringIndex + 1);

                        StringIndex = NewLine.ToString().IndexOf("Реле:");
                        if (StringIndex == -1)
                            continue;

                        if (byte.TryParse(NewLine.ToString().Substring(0, StringIndex), out NumValue) == false)
                            continue;

                        DayEvent.Minutes = NumValue;
                        NewLine.Remove(0, StringIndex + 5);

                        StringIndex = NewLine.ToString().IndexOf(";");
                        if (StringIndex == -1)
                            continue;

                        int IndexInArray = Array.IndexOf(EventWindow.RelayStatesStrings, NewLine.ToString().Substring(0, StringIndex));
                        if (IndexInArray == -1)
                            continue;

                        DayEvent.RelayState = EventWindow.RelayStatesStrings[IndexInArray];
                        Months[MonthIndex].Days[DayIndex].Events.Add(DayEvent);                        
                    }
                }
            }

            EventsCountUpdate();
            MonthEventsCountUpdate();
        }

        private void WindowIsClosing(object sender, CancelEventArgs e)
        {            
            ProgressBar.Close();
            USB_device.Close();
        }

        private void DropFile(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

                if (EventsWarningResult() == true)
                    OpenFile(files[0]);
            }
        }

        private void FilePreviewDragOver(object sender, DragEventArgs e)
        {
            bool dropEnabled = true;
            if (e.Data.GetDataPresent(DataFormats.FileDrop, true))
            {
                string[] filenames = e.Data.GetData(DataFormats.FileDrop, true) as string[];

                foreach (string filename in filenames)
                {
                    if (System.IO.Path.GetExtension(filename).ToUpperInvariant() != ".TXT")
                    {
                        dropEnabled = false;
                        break;
                    }
                }
            }
            else
            {
                dropEnabled = false;
            }

            if (!dropEnabled)
            {
                e.Effects = DragDropEffects.None;
                e.Handled = true;
            }
        }

        private void FilePreviewDragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effects = DragDropEffects.Copy;
            }
            else
            {
                e.Effects = DragDropEffects.None;
            }
        }
        
        private void DeleteAllButton_Click(object sender, RoutedEventArgs e)
        {
            DeleteAllEvents();
            foreach (Month MonthItem in Months)
                MonthItem.MonthEvents = 0;
        }

        private void DeleteAllEvents()
        {
            foreach (Month MonthItem in Months)
                DeleteMonthEvents(MonthItem);

            TotalEventsCount = 0;
            TotalCount.Text = "Всего событий: " + TotalEventsCount;
        }
        
        private void DownloadStart()
        {
            ThreadMonth = 0;
            ThreadDay = 0;
            ThreadEventInDay = 0;
            ThreadEvent = 0;

            int EventsSum = TotalEventsCount;
            if (EventsSum > EVENTS_COUNT_MAX)
                EventsSum = EVENTS_COUNT_MAX;

            byte[] USBPacket = new byte[USB_PACKET_SIZE];
            USBPacket[0] = USB_REPORT_ID;
            USBPacket[USB_CMD_POS] = USB_CMD_EEPROM_WRITE;
            USBPacket[USB_EEPROM_SUBCMD_POS] = EEPROM_SUBCMD_EVENTS_COUNT;
            USBPacket[USB_ACTIVE_EVENT_POS] = (byte)EventsSum;
            USBPacket[USB_ACTIVE_EVENT_POS + 1] = (byte)(EventsSum >> 8);            
            USBPacket[USB_START_STOP_FLAG_POS] = USB_CMD_START_FLAG;
            Boolean USBSuccess = USB_device.Write(USBPacket);

            ShowConnectionState(USBSuccess);
            if (USBSuccess == false)
                return;

            if (TotalEventsCount != 0)
            {
                FindNextEvent();
                ProgressBar.Owner = this;
                ProgressBar.Show();
                ThreadWrite = true;
                ThreadTimer.Start();
            }

            else
            {
                USBPacket[USB_EEPROM_SUBCMD_POS] = 0xFF; //нет команды
                USBPacket[USB_START_STOP_FLAG_POS] = USB_CMD_STOP_FLAG;
                USBSuccess = USB_device.Write(USBPacket);
            }
        }

        private void ClearConfigButton_Click(object sender, RoutedEventArgs e)
        {
            DeleteAllEvents();
            DownloadStart();
        }

        private void ShowConnectionState(bool State)
        {
            if (State)
            {
                StateImage.Source = new BitmapImage(new Uri("images/apply_32x32.png", UriKind.Relative));
                StateTextBlock.Text = DeviceConnectedString;
            }

            else
            {
                StateImage.Source = new BitmapImage(new Uri("images/warning_32x32.png", UriKind.Relative));
                StateTextBlock.Text = DeviceNotConnectedString;
            }

            DownloadMenu.IsEnabled = State;
            UploadMenu.IsEnabled = State;
            ClearMenu.IsEnabled = State;
            TimeMenu.IsEnabled = State;
        }

        private void MonthName_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            MonthEventsCountUpdate();
            
            EventsList.ItemsSource = null;

            MonthView.Visibility = Visibility.Collapsed;
            YearView.Visibility = Visibility.Visible;            
        }

        private void MonthBorder_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Border MonthClicked = (sender as Border);
            
            MonthDays.ItemsSource = Months[GetMonthIndex(MonthClicked.Uid)].Days;
            MonthName.Text = MonthClicked.Uid;
            MonthDays.SelectedItem = Months[GetMonthIndex(MonthClicked.Uid)].Days[0];
            EventsList.ItemsSource = Months[GetMonthIndex(MonthClicked.Uid)].Days[0].Events;

            MonthView.Visibility = Visibility.Visible;
            YearView.Visibility = Visibility.Collapsed;            
        }

        private void DayBorder_MouseLeftButtonClick(object sender, MouseButtonEventArgs e)
        {
            Border DayClicked = (sender as Border);
            EventsList.ItemsSource = Months[GetMonthIndex(MonthName.Text)].Days[byte.Parse(DayClicked.Uid) - 1].Events;            
        }

        private byte GetMonthIndex(string MonthName)
        {
            return ((byte)Array.IndexOf(MonthNames, MonthName));
        }

        private bool AreThereNoEvents()
        {
            foreach (Month MonthItem in Months)
            {
                foreach (MonthDay DayItem in MonthItem.Days)
                {
                    if (DayItem.Events.Count != 0)
                        return false;
                }
            }

            return true;
        }

        private bool EventsWarningResult()
        {
            if (AreThereNoEvents() == false)
            {
                WarningWindow Warning = new WarningWindow();
                Warning.Owner = this;
                if (Warning.ShowDialog() == true)
                    return true;
                else
                    return false;
            }

            return true;
        }

        private bool FindNextEvent()
        {            
            for (byte ActiveMonth = ThreadMonth; ActiveMonth < MonthsCount; ActiveMonth++)
            {
                for (byte ActiveDay = ThreadDay; ActiveDay < Months[ActiveMonth].DaysCount; ActiveDay++)
                {
                    if ((Months[ActiveMonth].Days[ActiveDay].Events.Count != 0) && (ThreadEventInDay < Months[ActiveMonth].Days[ActiveDay].Events.Count))
                    {
                        ThreadDay = ActiveDay;
                        ThreadMonth = ActiveMonth;
                        return true;
                    }

                    else
                        ThreadEventInDay = 0;                    
                }

                ThreadDay = 0;
            }

            return false;
        }

        private void EventsCountUpdate()
        {
            TotalEventsCount = 0;

            foreach (Month MonthItem in Months)
            {
                foreach (MonthDay DayItem in MonthItem.Days)
                    TotalEventsCount += DayItem.Events.Count;
            }

            TotalCount.Text = "Всего событий: " + TotalEventsCount;
        }

        private void MonthEventsCountUpdate()
        {
            foreach (Month MonthItem in Months)
            {
                MonthItem.MonthEvents = 0;

                foreach (MonthDay DayItem in MonthItem.Days)
                    MonthItem.MonthEvents += DayItem.Events.Count;
            }
        }

        private void CopyMonthEvents_Click(object sender, RoutedEventArgs e)
        {
            MonthCopyBuffer = new Month((MonthsList.SelectedItem as Month).Name, (MonthsList.SelectedItem as Month).DaysCount);
            CopyMonthEvents((MonthsList.SelectedItem as Month), MonthCopyBuffer, MonthCopyBuffer.DaysCount);
        }

        private void CopyMonthEvents(Month SourceMonth, Month DestinationMonth, int DaysCount)
        {
            for (int dayNum = 0; dayNum < DaysCount; dayNum++)
            {
                foreach (Event EventItem in SourceMonth.Days[dayNum].Events)
                    DestinationMonth.Days[dayNum].Events.Add(new Event() { Hours = EventItem.Hours, Minutes = EventItem.Minutes, RelayState = EventItem.RelayState, IsActive = true });
            }            
        }

        private void PasteMonthEvents_Click(object sender, RoutedEventArgs e)
        {
            if (MonthCopyBuffer != null)
            {
                Month DestinationMonth = (MonthsList.SelectedItem as Month);

                DeleteMonthEvents(DestinationMonth);
                
                int DaysToCopy = MonthCopyBuffer.DaysCount;
                if (DaysToCopy > DestinationMonth.DaysCount)
                    DaysToCopy = DestinationMonth.DaysCount;

                CopyMonthEvents(MonthCopyBuffer, DestinationMonth, DaysToCopy);

                DestinationMonth.MonthEvents = 0;

                foreach (MonthDay DayItem in DestinationMonth.Days)
                    DestinationMonth.MonthEvents += DayItem.Events.Count;

                EventsCountUpdate();
            }
        }

        private void DeleteMonthEvents_Click(object sender, RoutedEventArgs e)
        {
            DeleteMonthEvents(MonthsList.SelectedItem as Month);
            (MonthsList.SelectedItem as Month).MonthEvents = 0;
            EventsCountUpdate();
        }

        private void DeleteMonthEvents(Month SourceMonth)
        {
            foreach (MonthDay DayItem in SourceMonth.Days)
                DayItem.Events.Clear();
        }

        private void MonthBorder_ContextMenuOpening(object sender, ContextMenuEventArgs e)
        {
            Month SelectedMonth = (MonthsList.SelectedItem as Month);
            MenuItem MonthCopy = ((sender as Border).ContextMenu.Items[MENU_MONTH_COPY_INDEX] as MenuItem);
            MenuItem MonthDelete = ((sender as Border).ContextMenu.Items[MENU_MONTH_DELETE_INDEX] as MenuItem);
            MenuItem MonthPaste = ((sender as Border).ContextMenu.Items[MENU_MONTH_PASTE_INDEX] as MenuItem);


            if (SelectedMonth.MonthEvents != 0)
            {
                MonthCopy.IsEnabled = true;
                MonthDelete.IsEnabled = true;
            }

            else
            {
                MonthCopy.IsEnabled = false;
                MonthDelete.IsEnabled = false;
            }

            if (MonthCopyBuffer != null)
                MonthPaste.IsEnabled = true;
            else
                MonthPaste.IsEnabled = false;
        }
       
        private void DayBorder_ContextMenuOpening(object sender, ContextMenuEventArgs e)
        {
            MonthDay SelectedDay = (MonthDays.SelectedItem as MonthDay);
            MenuItem DayNew = ((sender as Border).ContextMenu.Items[MENU_EVENT_NEW_INDEX] as MenuItem);
            MenuItem DayCopy = ((sender as Border).ContextMenu.Items[MENU_EVENT_COPY_INDEX] as MenuItem);
            MenuItem DayPaste = ((sender as Border).ContextMenu.Items[MENU_EVENT_PASTE_INDEX] as MenuItem);
            MenuItem DayDelete = ((sender as Border).ContextMenu.Items[MENU_EVENT_DELETE_INDEX] as MenuItem);

            if (MonthDays.SelectedItems.Count == 1)
            {                
                EventsList.ItemsSource = Months[GetMonthIndex(MonthName.Text)].Days[MonthDays.SelectedIndex].Events;

                DayNew.IsEnabled = true;

                if (SelectedDay.Events.Count != 0)
                {
                    DayCopy.IsEnabled = true;
                    DayDelete.IsEnabled = true;
                }

                else
                {
                    DayCopy.IsEnabled = false;
                    DayDelete.IsEnabled = false;
                }                
            }

            else
            {
                DayCopy.IsEnabled = false;
                DayDelete.IsEnabled = true;
                DayNew.IsEnabled = false;
            }

            if (EventsCopyBuffer.Count != 0)
                DayPaste.IsEnabled = true;
            else
                DayPaste.IsEnabled = false;
        }        
    }
}
