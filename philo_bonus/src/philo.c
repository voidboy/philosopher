#include "philo.h"
#include "utils.h"
#include "inits.h"

void	ft_show_state(t_philo *p, t_states s)
{
	static const char	*states[] = {"has taken a fork", "is eating",
		"is sleeping", "is thinking", "died"};

	sem_wait(p->options.screen);
	if (!p->options.stop_simulation)
		printf("%5ld %3d %s\n", ft_delta(p->start), p->id + 1, states[s]);
	sem_post(p->options.screen);
}

void	ft_philosopher(t_philo *p)
{
	if (!(!p->id || !(p->id % 2)))
		usleep(p->options.time_to_eat);
	while (!p->options.stop_simulation && (p->options.must_eat == -1
			|| p->numb_meal < p->options.must_eat))
	{
		sem_wait(p->forks);
		ft_show_state(p, FORK);
		sem_wait(p->forks);
		ft_show_state(p, FORK);
		ft_show_state(p, EAT);
		if (++p->numb_meal == p->options.must_eat)
			break ;
		gettimeofday(&p->last_meal, NULL);
		usleep(p->options.time_to_eat);
		sem_post(p->forks);
		sem_post(p->forks);
		ft_show_state(p, SLEEP);
		usleep(p->options.time_to_sleep);
		ft_show_state(p, THINK);
	}
	sem_post(p->forks);
	sem_post(p->forks);
	exit(0);
}

void	*ft_monitoring(void *p)
{
	t_philo	*philo;

	philo = p;
	while (1)
	{
		if (ft_delta(philo->last_meal) >= philo->options.time_to_die)
		{
			sem_wait(philo->options.screen);
			philo->options.stop_simulation = TRUE;
			if (!(philo->numb_meal == philo->options.must_eat))
			{
				printf("%5ld %3d died\n",
					ft_delta(philo->start), philo->id + 1);
				ft_kill_all_philo(philo);
			}
			sem_post(philo->options.screen);
			exit(0);
		}
		usleep(1e3);
	}
}

void	start_meal(t_opts options, pthread_t *threads, t_philo *philo)
{
	pid_t	pid;
	int		i;

	i = -1;
	while (++i < options.number_of_philosophers)
	{
		pid = fork();
		if (pid == 0)
		{
			pthread_create(&threads[i], NULL, ft_monitoring, &philo[i]);
			ft_philosopher(&philo[i]);
		}
		philo[i].pid = pid;
	}
	i = -1;
	while (++i < options.number_of_philosophers)
		waitpid(philo[i].pid, NULL, 0);
}

int	main(int argc, char *argv[])
{
	t_opts			options;
	pthread_t		*threads;
	t_philo			*philo;
	sem_t			*forks;

	if ((argc != 5 && argc != 6) || !is_valid_options(argv, &options))
		return (1);
	if (ft_init_sem(&options, &forks) == -1
		|| ft_alloc(options, &threads, &philo) == -1
		|| ft_init_philo(options, philo, forks) == -1)
		return (1);
	start_meal(options, threads, philo);
	return (0);
}
