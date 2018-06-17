#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include <time.h> 
#include <locale.h> 
#include <conio.h>
#include <omp.h>
#include <intrin.h>

#define min 100		// Минимальное число прямоугольников
#define max 10000000   // Максимальное число прямоугольников
#define repeat (max * 1)   // Максимум повторов для инимальное число прямоугольников


#define ESC 27   // Код символа выхода из повторения

double fu(double x)
{
	return exp(x);
}
void main()
{
	double step, sum, x, y = 0, func1 = 0, time = 0;

	clock_t ms;
	long numSteps, numRepeat, i, mcs;
	// Контроль поддержки OpenMP
#ifndef _OPENMP 
	printf("OpenMP is not supported!\n");getchar();exit(1);
#endif 
	printf("\nTime in microseconds");
	// Цикл для наблюдения за повторяемостью 
	do
	{  	// Заголовок таблицы  результатов
		printf("\n========");
		// Вывод значений гранулярности (число прямоугольников)
		printf("\n Gran=>");
		for (numSteps = min; numSteps <= max; numSteps *= 10)
			printf("\t%7d", numSteps);
		printf("\nThreads\t------------------------------------------------");
		// Для каждого количества потоков выполняются вычисления с разной гранулярностью, задаваемой numSteps
		for (int numThreads = 1; numThreads <= 8; numThreads++)
		{
			printf("\n%5d", numThreads);
			for (long numSteps = min; numSteps <= max; numSteps *= 10)
			{   // Организация цикла повторов для обеспечения точности clock()
				numRepeat = repeat / numSteps;
				ms = clock();
				for (int countRepeat = 0; countRepeat < numRepeat; countRepeat++)
				{
					step = 1.0 / (double)numSteps;
					sum = 0;
					if (numThreads == 1)
					{  // исполнение без создания потоков
						for (x = 0.5*step; x <= 1.0; x += step)
							sum = sum + fu(x);
						y = step * sum;
						func1 = y; // Запоминаем как эталон для контроля многопотоковых вычислений
					}
					else // параллельное выполнение
					{
						// omp_set_num_threads(numThreads);
#pragma omp parallel for private(x) reduction(+:sum) num_threads(numThreads) 
						for (i = 0; i < numSteps; i++)
						{
							x = (i + 0.5)*step;
							sum = sum + fu(x);
						}
						y = step * sum;
					}
				}
				mcs = (clock() - ms) * 1000; // Вычислить время обработки в миллисекундах 
				if (fabs(y - func1) > 0.001) // Контроль результата с точностью до тысячных
					printf("\t  Error");
				else if (mcs / numRepeat < 100)
					printf("\t%3.2f", (float)mcs / float(numRepeat));
				else
					printf("\t%7d", mcs / numRepeat);

			}
		}
		printf("\n========================================================");
		printf("\n\nRepeat?( ESC - exit )\n");
	} while (_getch() != ESC); // Выход из цикла наблюдения за повторяемостью по символу минус '-'

}
