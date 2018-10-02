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
    public class MonthDay
    {
        public ObservableCollection<Event> Events { get; set; }
        public byte Number { get; }
        
        public MonthDay(byte DayNumber)
        {
            Number = DayNumber;
            Events = new ObservableCollection<Event>();
        }
    }
}
