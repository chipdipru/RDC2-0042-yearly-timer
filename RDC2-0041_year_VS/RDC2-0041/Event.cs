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
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RDC2_0041
{
    public class Event : INotifyPropertyChanged
    {
        private byte hours = 0;
        private byte minutes = 0;
        private string relaystate;
               
        public byte Hours
        {
            get { return this.hours; }
            set
            {
                if (this.hours != value)
                {
                    this.hours = value;
                    this.NotifyPropertyChanged("Hours");
                }
            }
        }

        public byte Minutes
        {
            get { return this.minutes; }
            set
            {
                if (this.minutes != value)
                {
                    this.minutes = value;
                    this.NotifyPropertyChanged("Minutes");
                }
            }
        }

        public string RelayState
        {
            get { return this.relaystate; }
            set
            {
                if (this.relaystate != value)
                {
                    this.relaystate = value;
                    this.NotifyPropertyChanged("RelayState");
                }
            }
        }
        
        public bool IsActive { get; set; }

        public event PropertyChangedEventHandler PropertyChanged;
        public void NotifyPropertyChanged(string propName)
        {
            this.PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propName));
        }
    }
}


