#include "philo.h"
#include "inits.h"
#include "utils.h"

void	ft_show_state(t_philo *p, t_states s)
{
	static const char	*states[] = {"has taken a fork", "is eating",
		"is sleeping", "is thinking", "died"};

	pthread_mutex_lock(&p->options->screen);
	if (!p->options->stop_simulation)
		printf("%5ld %3d %s\n", ft_delta(p->start), p->id + 1, states[s]);
	pthread_mutex_unlock(&p->options->screen);
}

void	*ft_philosopher(void *philo)
{
	t_philo	*p;

	p = (t_philo *)philo;
	if (!(!p->id || !(p->id % 2)))
		usleep(p->options->time_to_eat);
	while (!p->options->stop_simulation)
	{
		pthread_mutex_lock(p->forkR);
		ft_show_state(p, FORK);
		pthread_mutex_lock(p->forkL);
		ft_show_state(p, FORK);
		ft_show_state(p, EAT);
		gettimeofday(&p->last_meal, NULL);
		usleep(p->options->time_to_eat);
		if (++p->numb_meal == p->options->must_eat)
			break ;
		pthread_mutex_unlock(p->forkR);
		pthread_mutex_unlock(p->forkL);
		ft_show_state(p, SLEEP);
		usleep(p->options->time_to_sleep);
		ft_show_state(p, THINK);
	}
	pthread_mutex_unlock(p->forkR);
	pthread_mutex_unlock(p->forkL);
	return (NULL);
}

void	ft_start_meal(pthread_t *threads, t_philo *philo)
{
	int	i;

	i = 0;
	while (i < philo->options->number_of_philosophers)
	{
		pthread_create(&threads[i], NULL, ft_philosopher, &philo[i]);
		pthread_detach(threads[i]);
		i += 2;
	}
	i = 1;
	while (i < philo->options->number_of_philosophers)
	{
		pthread_create(&threads[i], NULL, ft_philosopher, &philo[i]);
		pthread_detach(threads[i]);
		i += 2;
	}
}

void	ft_monitoring(t_philo *philo)
{
	int		i;

	while (1)
	{
		i = -1;
		while (++i < philo->options->number_of_philosophers)
		{
			if (ft_delta(philo[i].last_meal) >= philo[i].options->time_to_die)
			{
				pthread_mutex_lock(&philo[i].options->screen);
				philo[i].options->stop_simulation = TRUE;
				if (!(philo[i].numb_meal == philo[i].options->must_eat))
					printf("%5ld %3d died\n",
						ft_delta(philo->start), philo[i].id + 1);
				pthread_mutex_unlock(&philo[i].options->screen);
				return ;
			}
		}
		usleep(1e3);
	}
}

int	main(int argc, char *argv[])
{
	t_opts			*options;
	pthread_t		*threads;
	t_philo			*philo;
	pthread_mutex_t	*forks;

	options = malloc(sizeof(t_opts));
	if (!options || (argc != 5 && argc != 6)
		|| !is_valid_options(argv, options))
		return (1);
	options->stop_simulation = FALSE;
	if (pthread_mutex_init(&options->screen, NULL) == -1)
	{
		write(STDERR_FILENO, "pthread_mutex_init error\n", 25);
		free(options);
		return (1);
	}
	if (ft_alloc(options, &threads, &philo, &forks) == -1
		|| ft_init_philo(options, threads, philo, forks) == -1)
		return (1);
	ft_start_meal(threads, philo);
	ft_monitoring(philo);
	ft_free_philo(threads, philo, forks, options);
	return (0);
}
