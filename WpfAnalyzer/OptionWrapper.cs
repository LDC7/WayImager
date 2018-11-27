namespace WpfAnalyzer
{
    using RouteAnalyzer;
    using System;
    using System.ComponentModel;
    using System.Runtime.CompilerServices;
    public class OptionWrapper : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private int _densityWinSize;
        private int _dispWinSize;
        private int _widthCamera;
        private int _heightCamera;
        private byte _minLvlForDensityMap;
        private byte _maxLvlForDensityMap;
        private int _coefficientForDensity;
        private int _coefficientForDispersion;
        private int _minLimitForInfMap;
        private int _maxIters;
        public OptionWrapper(Options options)
        {
            _densityWinSize = options.MapWinSize;
            _dispWinSize = (int)options.DispWinSize;
            _widthCamera = options.CameraWidth;
            _heightCamera = options.CameraHeight;
            _minLvlForDensityMap = options.MinLvlForDensityMap;
            _maxLvlForDensityMap = options.MaxLvlForDensityMap;
            _coefficientForDensity = options.CoefficientForDensity;
            _coefficientForDispersion = options.CoefficientForDispersion;
            _minLimitForInfMap = options.MinLimitForInfMap;
        }
        public string densityWinSize
        {
            get { return _densityWinSize.ToString(); }
            set
            {
                int val;
                if (int.TryParse(value, out val))
                {
                    _densityWinSize = val;
                    NotifyPropertyChanged("densityWinSize");
                }
            }
        }
        public string dispWinSize
        {
            get { return _dispWinSize.ToString(); }
            set
            {
                int val;
                if (int.TryParse(value, out val))
                {
                    _dispWinSize = val;
                    NotifyPropertyChanged("dispWinSize");
                }
            }
        }
        public string widthCamera
        {
            get { return _widthCamera.ToString(); }
            set
            {
                int val;
                if (int.TryParse(value, out val))
                {
                    _widthCamera = val;
                    NotifyPropertyChanged("widthCamera");
                }
            }
        }
        public string heightCamera
        {
            get { return _heightCamera.ToString(); }
            set
            {
                int val;
                if (int.TryParse(value, out val))
                {
                    _heightCamera = val;
                    NotifyPropertyChanged("heightCamera");
                }
            }
        }
        public string minLvlForDensityMap
        {
            get { return _minLvlForDensityMap.ToString(); }
            set
            {
                byte val;
                if (byte.TryParse(value, out val))
                {
                    _minLvlForDensityMap = val;
                    NotifyPropertyChanged("minLvlForDensityMap");
                }
            }
        }
        public string maxLvlForDensityMap
        {
            get { return _maxLvlForDensityMap.ToString(); }
            set
            {
                byte val;
                if (byte.TryParse(value, out val))
                {
                    _maxLvlForDensityMap = val;
                    NotifyPropertyChanged("maxLvlForDensityMap");
                }
            }
        }
        public string coefficientForDensity
        {
            get { return _coefficientForDensity.ToString(); }
            set
            {
                int val;
                if (int.TryParse(value, out val))
                {
                    _coefficientForDensity = val;
                    NotifyPropertyChanged("coefficientForDensity");
                }
            }
        }
        public string coefficientForDispersion
        {
            get { return _coefficientForDispersion.ToString(); }
            set
            {
                int val;
                if (int.TryParse(value, out val))
                {
                    _coefficientForDispersion = val;
                    NotifyPropertyChanged("coefficientForDispersion");
                }
            }
        }
        public string minLimitForInfMap
        {
            get { return _minLimitForInfMap.ToString(); }
            set
            {
                int val;
                if (int.TryParse(value, out val))
                {
                    _minLimitForInfMap = val;
                    NotifyPropertyChanged("minLimitForInfMap");
                }
            }
        }
        private void NotifyPropertyChanged([CallerMemberName] String propertyName = "")
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        public Options GetOptions()
        {
            Options op = new Options();
            op.MapWinSize = _densityWinSize;
            op.DispWinSize = _dispWinSize;
            op.CameraWidth = _widthCamera;
            op.CameraHeight = _heightCamera;
            op.MinLvlForDensityMap = _minLvlForDensityMap;
            op.MaxLvlForDensityMap = _maxLvlForDensityMap;
            op.CoefficientForDensity = _coefficientForDensity;
            op.CoefficientForDispersion = _coefficientForDispersion;
            op.MinLimitForInfMap = _minLimitForInfMap;
            return op;
        }
    }
}