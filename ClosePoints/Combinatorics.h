class Combinatorics
{
public:
	Combinatorics(int n, int m)
	{
		this->n = n;
		this->m = m;
		a = GenerateArray(m);
	}

	int Size()
	{
		return m;
	}

	int Value(int index)
	{
		return a[index] - 1;
	}

	bool NextSet()
	{
		int k = m;
		for (int i = k - 1; i >= 0; --i)
			if (a[i] < n - k + i + 1)
			{
				++a[i];
				for (int j = i + 1; j < k; ++j)
					a[j] = a[j - 1] + 1;
				return true;
			}
		return false;
	}

private:

	int n;

	int m;

	int *a;

	int* GenerateArray(int n)
	{
		int *a = new int[n];

		for (int i = 0; i < n; i++)
		{
			a[i] = i + 1;
		}

		return a;
	}
};






void RemoveArray(int *a)
{
	delete[] a;
}