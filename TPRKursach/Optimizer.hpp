#pragma once
#include <Eigen/Geometry>
#include <functional>
#include <vector>
#include "Configuration.h"

#ifndef NEDLER_MID_ALPHA
#define NEDLER_MID_ALPHA 1.0
#endif // !NEDLER_MID_ALPHA

#ifndef NEDLER_MID_BETA
#define NEDLER_MID_BETA 0.5
#endif // !NEDLER_MID_BETA

#ifndef NEDLER_MID_GAMMA
#define NEDLER_MID_GAMMA 2.0
#endif // !NEDLER_MID_GAMMA

#include "erfinv.h"

template<int N>
class Optimizer
{
public:
	Optimizer(double epsilon,
			  size_t max_iter,
			  double nm_alpha = NEDLER_MID_ALPHA,
			  double nm_beta = NEDLER_MID_BETA,
			  double nm_gamma = NEDLER_MID_GAMMA
	)
		:
		epsilon_(epsilon),
		max_iter_(max_iter),
		nm_alpha_(nm_alpha),
		nm_beta_(nm_beta),
		nm_gamma_(nm_gamma)
	{
	}

	~Optimizer()
	{
	}

	Optimizer& set_optimal_functor(const std::function<bool(double a, double b)>& optimal)
	{
		optimal_ = optimal;
		return *this;
	}

	void optimize(const ModelData& md, Eigen::Matrix<double, N, 1>& opt_arg)
	{
		std::vector<std::function<double(const Eigen::Matrix<double, N, 1>&)>> gs;
		auto f = [&md](const Eigen::Matrix<double, N, 1>& x)
		{
			double res = .0;
			for(size_t i = 0; i < md.c.size(); ++i)
				for(size_t j = 0; j < md.c.at(0).size(); ++j)
				{
					res += md.c.at(i).at(j) * x[i * md.c.at(0).size() + j];
				}
			return res;
		};

		for(size_t k = 0; k < md.a.size(); ++k)
			for(size_t i = 0; i < md.c.size(); ++i)
			{
				gs.emplace_back([&md, k, i](const Eigen::Matrix<double, N, 1>& x)
								{
									double res = -md.b.at(k).at(i);
									double sum = 0.0;
									for(size_t j = 0; j < md.a.at(k).at(i).size(); ++j)
									{
										res +=
											(md.a.at(k).at(i).at(j).first + md.a.at(k).at(i).at(j).second)
											/ 2;
										sum +=
											std::pow(x[i * md.c.at(0).size() + j], 2) *
											std::pow(md.a.at(k).at(i).at(j).second - md.a.at(k).at(i).at(j).first, 4)
											/ 144
											;
									}

									res += std::pow(sum, 0.5) * erfinv(md.alpha);

									return -res;
								}
				);
			}

		for(size_t j = 0; j < md.d.size(); ++j)
		{
			gs.emplace_back([&md, j](const Eigen::Matrix<double, N, 1>& x)
							{
								double res = -md.d.at(j);
								for(size_t i = 0; i < md.c.size(); ++i)
								{
									res += x[i * md.c.at(0).size() + j];
								}

								return res;
							}
			);
		}

		for(size_t i = 0; i < md.c.size(); ++i)
			for(size_t j = 0; j < md.d.size(); ++j)
			{
				gs.emplace_back([&md, i, j](const Eigen::Matrix<double, N, 1>& x)
								{
									return x[i * md.c.at(0).size() + j];
								}
				);
			}

		for(size_t j = 0; j < md.d.size(); ++j)
			for(size_t i = 0; i < md.c.size(); ++i)
			{
				opt_arg[i * md.c.at(0).size() + j] = md.d[j];
			}

		penalty_(f, gs, opt_arg);
#ifdef OPTIMIZER_CMD_DEBUG_OUTPUT
		std::cout << "Optimal value: " << f(opt_arg) << std::endl;
#endif // OPTIMIZER_CMD_DEBUG_OUTPUT
	}


private:
	std::function<bool(double a, double b)> optimal_ = [](double a, double b)
	{
		return a < b;
	}; // true if a better than b, minimize by default

	void penalty_(const std::function<double(const Eigen::Matrix<double, N, 1>&)>& f,
				  const std::vector<std::function<double(const Eigen::Matrix<double, N, 1>&)>>& gs,
				  Eigen::Matrix<double, N, 1>& opt_arg
	)
	{
		double r = 10000.0; // starting penalty coeff
		double beta = 0.9;

		auto f1 = [&gs, &f, &r](const Eigen::Matrix<double, N, 1>& x)
		{
			auto res = f(x);
			for(auto& g : gs)
			{
				res += r * (1 / g(x));
			}
			return res;
		};

		double sum;
		do
		{
			nedler_mid_(f1, opt_arg);
			r *= beta;
			sum = .0;
			for(auto& g : gs)
			{
				sum += r * (1 / g(opt_arg));
			}
		} while(sum > 0.00001);

	}

	void nedler_mid_(const std::function<double(const Eigen::Matrix<double, N, 1>&)>& f,
					 Eigen::Matrix<double, N, 1>& opt_arg
	)
	{
		const auto n = opt_arg.size();

		std::vector<Eigen::Matrix<double, N, 1>> simplex;
		simplex.emplace_back(Eigen::Matrix<double, N, 1>::Zero());

		for(int i = 0; i < n; ++i)
		{
			simplex.emplace_back(simplex.at(i) + Eigen::Matrix<double, N, 1>::Unit(i) * (simplex.at(0).coeff(i) != 0 ? 0.05 : 0.00025));
		}

		typename std::vector<Eigen::Matrix<double, N, 1>>::iterator pBetter = simplex.begin();
		typename std::vector<Eigen::Matrix<double, N, 1>>::iterator pGood = simplex.begin();
		typename std::vector<Eigen::Matrix<double, N, 1>>::iterator pBad = simplex.begin();

		size_t iterations_counter = 0;
		while(iterations_counter++ < max_iter_ && !optimal_(f(*pBetter), epsilon_))
		{
#ifdef OPTIMIZER_CMD_DEBUG_OUTPUT_NEDLER_MID
			std::cout << " x_opt = " << *pBetter << " it " + std::to_string(iterations_counter) + ", f = " + std::to_string(f(*pBetter)) << std::endl;
#endif // OPTIMIZER_CMD_DEBUG_OUTPUT_NEDLER_MID
			for(auto pVertex = simplex.begin(); pVertex != simplex.end(); ++pVertex)
			{
				if(optimal_(f(*pBad), f(*pVertex)))
				{
					pBad = pVertex;
				}
				else if(optimal_(f(*pGood), f(*pVertex)))
				{
					pGood = pVertex;
				}

				if(optimal_(f(*pVertex), f(*pBetter)))
				{
					pBetter = pVertex;
				}

			}

			Eigen::Matrix<double, N, 1> center_gravity = Eigen::Matrix<double, N, 1>::Zero();

			for(auto pVertex = simplex.begin(); pVertex != simplex.end(); ++pVertex)
			{
				if(pVertex != pBad)
				{
					center_gravity += *pVertex;
				}
			}
			center_gravity /= simplex.size() - 1;

			Eigen::Matrix<double, N, 1> reflected = (1 + nm_alpha_) * center_gravity - nm_alpha_ * *pBad;

			if(optimal_(f(reflected.eval()), f(*pBetter)))
			{
				auto stretched = (1 - nm_gamma_) * center_gravity + nm_gamma_ * reflected;
				if(optimal_(f(stretched.eval()), f(*pBetter)))
				{
					*pBad = stretched;
				}
				else
				{
					*pBad = reflected;
				}
			}
			else if(optimal_(f(reflected), f(*pGood)))
			{
				*pBad = reflected;
			}
			else
			{
				if(optimal_(f(reflected), f(*pBad)))
				{
					auto temp = reflected;
					reflected = *pBad;
					*pBad = temp;
				}

				auto compressed = nm_beta_ * *pBad + (1 - nm_beta_) * center_gravity;
				if(optimal_(f(compressed), f(*pBad)))
				{
					*pBad = compressed;
				}
				else
				{
					for(auto pVertex = simplex.begin(); pVertex != simplex.end(); ++pVertex)
					{
						*pVertex = *pBetter + (*pVertex - *pBetter) / 2;
					}
				}
			}
		}

		opt_arg = *pBetter;
	}

	double nm_alpha_;
	double nm_beta_;
	double nm_gamma_;

	double epsilon_;
	size_t max_iter_;
};

