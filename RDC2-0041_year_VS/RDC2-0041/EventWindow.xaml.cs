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
    /// Interaction logic for EventWindow.xaml
    /// </summary>
    public partial class EventWindow : Window
    {
        
        public static readonly string[] RelayStatesStrings =
        {
            "отключено",
            "включено"
        };

        private Event EventToAdd = new Event();
        public ObservableCollection<string> RelayStates { get; }
        
        public EventWindow(Event NewEvent)
        {
            InitializeComponent();

            EventToAdd = NewEvent;
            
            RelayStates = new ObservableCollection<string>(RelayStatesStrings);
            
            DataContext = this;

            if (EventToAdd.IsActive == true)
            {
                HoursText.Text = EventToAdd.Hours.ToString();
                MinutesText.Text = EventToAdd.Minutes.ToString();
                RelayStateCombo.SelectedIndex = RelayStates.IndexOf(EventToAdd.RelayState);                
                EventToAdd.IsActive = false;
            }            
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void AddButton_Click(object sender, RoutedEventArgs e)
        {
            EventToAdd.IsActive = true;
            byte TimeVal;
            byte.TryParse(HoursText.Text, out TimeVal);
            EventToAdd.Hours = TimeVal;
            byte.TryParse(MinutesText.Text, out TimeVal);
            EventToAdd.Minutes = TimeVal;
            EventToAdd.RelayState = RelayStateCombo.SelectedItem as string;            
            Close();
        }
    }
}
