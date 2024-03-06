#include <stdio.h>
int main() {
	int n;
	scanf("%d", &n);
	int number[10] = {0};
	int sum = 0;
	while (n > 0) {
		number[sum++] = n % 10;
		n /= 10; 
	}
	int isP = 1;
	if (sum % 2 == 0) {
		int temp = 0;
		while (temp < sum / 2) {
			if (number[temp] != number[sum - temp - 1]) {
				isP = 0;
				break;
			}
			temp++;
		}
	} else {
		int temp = 0;
		while (temp < sum / 2) {
			if (number[temp] != number[sum - temp - 1]) {
				isP = 0;
				break;
			}
			temp++;
		}	
	}

	if (isP == 1) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
