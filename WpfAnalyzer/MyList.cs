namespace WpfAnalyzer
{
    using System;
    using System.Collections.Generic;

    public class MyList<T> : List<T>
    {
        public event EventHandler OnChangedCollection;

        public void Add(T item)
        {
            base.Add(item);
            OnChangedCollection?.Invoke(this, null);
        }

        public void RemoveAt(int index)
        {
            base.RemoveAt(index);
            OnChangedCollection?.Invoke(this, null);
        }

    }
}
