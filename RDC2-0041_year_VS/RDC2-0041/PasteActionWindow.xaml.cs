/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2017
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


using System;
using System.Collections.Generic;
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
    /// Interaction logic for PasteActionWindow.xaml
    /// </summary>
    public partial class PasteActionWindow : Window
    {
        public const int Skip = 0;
        public const int Replace = 1;

        public PasteActionWindow()
        {
            InitializeComponent();
        }

        public int Action { get; set; }

        private void SkipEventsButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
            Action = Skip;
        }

        private void ReplaceEventsButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
            Action = Replace;
        }
    }
}
