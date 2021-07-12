#include "philo.h"
#include "utils.h"

int	ft_fail_philo(int i, pthread_t *threads, t_philo *philo,
		pthread_mutex_t *forks)
{
	write(STDERR_FILENO, "pthread_mutex_init error\n", 25);
	while (--i >= 0)
		pthread_mutex_destroy(&forks[i]);
	free(threads);
	free(philo);
	free(forks);
	return (-1);
}

void	ft_free_philo(pthread_t *threads, t_philo *philo,
		pthread_mutex_t *forks, t_opts *options)
{
	int	i;

	i = -1;
	while (++i < philo->options->number_of_philosophers)
		pthread_mutex_destroy(&forks[i]);
	free(threads);
	free(philo);
	free(forks);
	free(options);
}

int	ft_init_philo(t_opts *options, pthread_t *threads, t_philo *philo,
		pthread_mutex_t *forks)
{
	struct timeval	start;
	int				i;

	if (gettimeofday(&start, NULL) == -1)
		return (-1);
	i = -1;
	while (++i < options->number_of_philosophers)
		if (pthread_mutex_init(&forks[i], NULL) == -1)
			return (ft_fail_philo(i, threads, philo, forks));
	i = -1;
	while (++i < options->number_of_philosophers)
	{
		if (i == options->number_of_philosophers - 1)
			philo[i] = (t_philo){i, options, &forks[i], &forks[0 + 0],
				0, start, start};
		else
			philo[i] = (t_philo){i, options, &forks[i], &forks[i + 1],
				0, start, start};
	}
	return (0);
}

int	ft_alloc(t_opts *options, pthread_t **threads, t_philo **philo,
		pthread_mutex_t **forks)
{
	*threads = malloc(sizeof(pthread_t) * options->number_of_philosophers);
	if (!*threads)
	{
		free(options);
		return (-1);
	}
	*philo = malloc(sizeof(t_philo) * options->number_of_philosophers);
	if (!*philo)
	{
		free(options);
		free(*threads);
		return (-1);
	}
	*forks = malloc(sizeof(pthread_mutex_t) * options->number_of_philosophers);
	if (!*forks)
	{
		free(options);
		free(*threads);
		free(*philo);
		return (-1);
	}
	return (0);
}
