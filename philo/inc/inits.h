#ifndef INITS_H
# define INITS_H
int		ft_fail_philo(int i, pthread_t *threads, t_philo *philo,
		pthread_mutex_t *forks);
void	ft_free_philo(pthread_t *threads, t_philo *philo,
		pthread_mutex_t *forks, t_opts *options);
int		ft_init_philo(t_opts *options, pthread_t *threads, t_philo *philo,
		pthread_mutex_t *forks);
int		ft_alloc(t_opts *options, pthread_t **threads, t_philo **philo,
		pthread_mutex_t **forks);
#endif
