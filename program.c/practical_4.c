// Calculate area  and perimeter of a rectangle
#include<stdio.h>
int main()
{
    float length, width, area, perimeter;
    printf("enter length and width of rectangle:");
    scanf("%f %f",&length,&width);
    area = length * width;
    perimeter = 2 * (length + width);
    printf("Area of rectangle = %.2f\n", area);
    printf("Perimeter of rectangle = %.2f\n", perimeter);
    return 0;
}