int main() {
    int arr[] = {64, 34, 25, 12, 22, 11, 90};
   
	int tmp;
	int i= 3, j= 0;
   
	tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;

	return arr[0];
}