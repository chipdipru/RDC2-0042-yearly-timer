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
using System.Linq;
using System.Text;

namespace RDC2_0041
{
    public class TimeSettings
    {        
        public byte Seconds { get; set; }

        public byte Minutes { get; set; }

        public byte Hours { get; set; }

        public byte WeekDay { get; set; }

        public byte Date { get; set; }

        public byte Month { get; set; }

        public byte Year { get; set; }

        public byte TimeZoneId { get; set; }
    }
}
