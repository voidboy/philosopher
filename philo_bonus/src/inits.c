#include "philo.h"
#include "utils.h"

void	ft_kill_all_philo(t_philo *philo)
{
	int	id;
	int	preserve;

	preserve = philo->id;
	philo -= preserve;
	id = -1;
	while (++id < philo->options.number_of_philosophers)
		if (id != preserve)
			kill(philo[id].pid, SIGINT);
}

int	ft_init_sem(t_opts *options, sem_t **forks)
{
	options->stop_simulation = FALSE;
	*forks = sem_open("forks", O_CREAT | O_EXCL,
			0700, options->number_of_philosophers);
	if (*forks == SEM_FAILED)
	{
		sem_unlink("forks");
		*forks = sem_open("forks", O_CREAT | O_EXCL,
				0700, options->number_of_philosophers);
	}
	options->screen = sem_open("screen", O_CREAT | O_EXCL,
			0700, 1);
	if (options->screen == SEM_FAILED)
	{
		sem_unlink("screen");
		options->screen = sem_open("screen", O_CREAT | O_EXCL,
				0700, 1);
	}
	if (*forks == SEM_FAILED || options->screen == SEM_FAILED)
		return (-1);
	else
		return (0);
}

int	ft_init_philo(t_opts options, t_philo *philo, sem_t *forks)
{
	struct timeval	start;
	int				i;

	if (gettimeofday(&start, NULL) == -1)
		return (-1);
	i = -1;
	while (++i < options.number_of_philosophers)
		philo[i] = (t_philo){i, 0, options, forks, 0, start, start};
	return (0);
}

int	ft_alloc(t_opts options, pthread_t **threads, t_philo **philo)
{
	*threads = malloc(sizeof(pthread_t) * options.number_of_philosophers);
	if (!*threads)
	{
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	*philo = malloc(sizeof(t_philo) * options.number_of_philosophers);
	if (!*philo)
	{
		free(*threads);
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	return (0);
}
