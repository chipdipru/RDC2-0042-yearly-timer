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
using System.ComponentModel;
using System.Linq;
using System.Text;



namespace RDC2_0041
{
    public class Month : INotifyPropertyChanged
    {
        public ObservableCollection<MonthDay> Days { get; set; }
        private int monthevents = 0;

        public string Name { get; }
        public byte DaysCount { get; }

        public int MonthEvents
        {
            get { return monthevents; }
            set
            {
                if (monthevents != value)
                {
                    monthevents = value;
                    NotifyPropertyChanged("MonthEvents");
                }
            }
        }
        
        public Month (string MonthName, byte MonthDaysCount)
        {
            Name = MonthName;
            DaysCount = MonthDaysCount;
            Days = new ObservableCollection<MonthDay>();
            for(byte dayNum = 1; dayNum <= DaysCount; dayNum++)
            {
                Days.Add(new MonthDay(dayNum));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public void NotifyPropertyChanged(string propName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propName));
        }
    }
}
