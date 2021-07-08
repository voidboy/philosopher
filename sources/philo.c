#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

typedef enum e_bool {
	FALSE,
	TRUE,
}			t_bool;

typedef struct s_philo {
	int				philo;
	pthread_mutex_t forkL;
	pthread_mutex_t forkR;
	struct timeval	start;
	t_bool			alive;
}				t_philo;

long	ft_delta(struct timeval start)
{
	struct timeval	delta;

	if (gettimeofday(&delta, NULL) == -1)
	{
		write(STDERR_FILENO, "gettimeofday error\n", 19);
		return (-1);
	}
	return ((delta.tv_sec * 1e3 + delta.tv_usec / 1e3)
	- (start.tv_sec * 1e3 + start.tv_usec / 1e3));

}

void *ft_philosopher(void *philo)
{
	t_philo		*p = (t_philo *)philo;
	
	if (p->philo % 2)
	{
		while (1)
		{
			if (pthread_mutex_lock(&p->forkL) != 0)
				perror("OMG!\n");
			if (pthread_mutex_lock(&p->forkR) != 0)
				perror("OMG!\n");
			printf("%05ld %02d is eating\n", ft_delta(p->start), p->philo);
			usleep(3000000);
			if (pthread_mutex_unlock(&p->forkL) != 0)
				perror("OMG!\n");
			if (pthread_mutex_unlock(&p->forkR) != 0)
				perror("OMG!\n");
			printf("%05ld %02d is sleeping\n", ft_delta(p->start), p->philo);
			usleep(3000000);
			printf("%05ld %02d is thinking\n", ft_delta(p->start), p->philo);
		}

	}
	else 
	{
		printf("%05ld %02d is thinking\n", ft_delta(p->start), p->philo);
		usleep(3000000);
		while (1)
		{
			if (pthread_mutex_lock(&p->forkL) != 0)
				perror("OMG!\n");
			if (pthread_mutex_lock(&p->forkR) != 0)
				perror("OMG!\n");
			printf("%05ld %02d is eating\n", ft_delta(p->start), p->philo);
			usleep(3000000);
			if (pthread_mutex_unlock(&p->forkL) != 0)
				perror("OMG!\n");
			if (pthread_mutex_unlock(&p->forkR) != 0)
				perror("OMG!\n");
			printf("%05ld %02d is sleeping\n", ft_delta(p->start), p->philo);
			usleep(3000000);
			printf("%05ld %02d is thinking\n", ft_delta(p->start), p->philo);

		}
	}
	return (NULL);
}

int ft_init_philo(int n, pthread_mutex_t *forks, t_philo *philo)
{
	struct timeval	start;
	int				i;

	if (gettimeofday(&start, NULL) == -1)
		return (-1);
	i = -1;
	while (++i < n)
	{
		if (pthread_mutex_init(&forks[i], NULL) == -1)
		{
			write(STDERR_FILENO, "pthread_mutex_init error\n", 25);
			while (--i >= 0)
				pthread_mutex_destroy(&forks[i]);
			return (-1);
		}
	}
	i = -1;
	while (++i < n)
	{
		if (i == n - 1)
			philo[i] = (t_philo){i, forks[i], forks[0], start, TRUE};
		else 
			philo[i] = (t_philo){i, forks[i], forks[i + 1], start, TRUE};
	}
	return (0);
}

int ft_alloc(int n, pthread_t **threads, t_philo **philo, pthread_mutex_t **forks)
{
	*threads = malloc(sizeof(pthread_t) * n);
	if (!*threads)
	{
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	*philo = malloc(sizeof(t_philo) * n);
	if (!*philo)
	{
		free(*threads);
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	*forks = malloc(sizeof(pthread_mutex_t) * n);
	if (!*forks)
	{
		free(*threads);
		free(*philo);
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	return (0);
}

int main(int argc, char *argv[])
{
	int n = 2;

	pthread_t		*threads;
	t_philo			*philo;
	pthread_mutex_t *forks;


	if (ft_alloc(n, &threads, &philo, &forks) == -1
		|| ft_init_philo(n, forks, philo) == -1)
		return (1);
	pthread_create(&threads[0], NULL, ft_philosopher, (void *)&philo[0]);
	pthread_detach(threads[0]);
	pthread_create(&threads[1], NULL, ft_philosopher, (void *)&philo[1]);
	pthread_detach(threads[1]);
	while (1)
		;
}
