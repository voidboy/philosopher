#ifndef INITS_H
# define INITS_H
void	ft_kill_all_philo(t_philo *philo);
int		ft_init_philo(t_opts options, t_philo *philo, sem_t *forks);
int		ft_init_sem(t_opts *options, sem_t **forks);
int		ft_alloc(t_opts options, pthread_t **threads, t_philo **philo);
#endif
