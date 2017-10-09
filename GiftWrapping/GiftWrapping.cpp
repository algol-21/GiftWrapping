#include "stdafx.h"
#include "GiftWrapping.h"


size_t us_hash(const Face& S)
{
	size_t sum = 0;
	// -----
	// for (auto item : S)
	for (auto item : S.m_data)
	// -----
		sum += item;
	return sum;
}

GiftWrapping::GiftWrapping()
{
	input_points = std::vector<MathVector>();

	scatter_points = std::vector<MathVector>();

	// -----
	// convex_hull = std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)>(0, us_hash);
	// convex_hull = std::unordered_set<std::set<size_t>, decltype(&us_hash)>(0, us_hash);
	// convex_hull = std::unordered_set<Face, decltype(&us_hash)>(0, us_hash);
	convex_hull = Faces();
	// -----
}


GiftWrapping::~GiftWrapping()
{
}

void GiftWrapping::run_algorithm()
{
	// /////
	unsigned int start_time = clock();
	// /////

	b_FinishAlgorithm = false;

	scatter_points = input_points;

	while (!b_FinishAlgorithm)
	{
		b_NonSimplicity = false;
		wrapping_algorithm();
	}

	// /////
	unsigned int end_time = clock();
	std::cout << "Wrapping time: " << (float)(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
	// /////
}

void GiftWrapping::wrapping_algorithm()
{
	convex_hull.m_data.clear();

	// Initialize remaining indexes of points, which can be considered.
	std::unordered_set<size_t> interest_indexes_of_points;
	//for (size_t counter = 0; counter < scatter_points.size(); ++counter)
	//	interest_indexes_of_points.insert(counter);

	// Declare queue of tagged hyperfaces and set of bounding subfaces.
	// -----
	// std::queue<std::unordered_set<size_t>> queue_of_hyperfaces;
	// std::queue<std::set<size_t>> queue_of_hyperfaces;
	std::queue<Face> queue_of_hyperfaces;
	// -----
	// -----
	// std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> bounding_subfaces(0, us_hash);
	// std::unordered_set<std::set<size_t>, decltype(&us_hash)> bounding_subfaces(0, us_hash);
	// std::unordered_set<Face, decltype(&us_hash)> bounding_subfaces(0, us_hash);
	Faces bounding_subfaces;
	// -----
	// Find first hyperface.
	// -----
	// std::unordered_set<size_t> current_hyperface;
	// std::set<size_t> current_hyperface;
	Face current_hyperface;
	// -----
	find_first_hyperface(current_hyperface);

	// Push first hyperface in queue and find its subfaces.
	queue_of_hyperfaces.push(current_hyperface);
	find_subfaces(current_hyperface, bounding_subfaces);

	// -----
	// std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> subfaces_of_current_hyperface(0, us_hash);
	// std::unordered_set<std::set<size_t>, decltype(&us_hash)> subfaces_of_current_hyperface(0, us_hash);
	// std::unordered_set<Face, decltype(&us_hash)> subfaces_of_current_hyperface(0, us_hash);
	Faces subfaces_of_current_hyperface;
	// -----

	// -----
	// std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> subfaces(0, us_hash);
	// std::unordered_set<std::set<size_t>, decltype(&us_hash)> subfaces(0, us_hash);
	// std::unordered_set<Face, decltype(&us_hash)> subfaces(0, us_hash);
	Faces subfaces;
	// -----

	// -----
	// std::unordered_set<size_t> new_hyperface;
	// std::set<size_t> new_hyperface;
	Face new_hyperface;
	// -----
	size_t new_vertex_index;	

	// -----
	// std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> intersection_of_subfaces(0, us_hash);
	// std::unordered_set<std::set<size_t>, decltype(&us_hash)> intersection_of_subfaces(0, us_hash);
	// std::unordered_set<Face, decltype(&us_hash)> intersection_of_subfaces(0, us_hash);
	Faces intersection_of_subfaces;
	// -----
	// -----
	// std::unordered_set<std::unordered_set<size_t>, decltype(&us_hash)> subfaces_of_new_hyperface(0, us_hash);
	// std::unordered_set<std::set<size_t>, decltype(&us_hash)> subfaces_of_new_hyperface(0, us_hash);
	// std::unordered_set<Face, decltype(&us_hash)> subfaces_of_new_hyperface(0, us_hash);
	Faces subfaces_of_new_hyperface;
	// -----

	std::vector<MathVector> cross_product_vectors;

	MathVector normal_of_hyperface;
	MathVector normal_of_subface;

	while (!queue_of_hyperfaces.empty())
	{
		// Pop hyperface from queue and find its subfaces.
		current_hyperface = queue_of_hyperfaces.front();
		queue_of_hyperfaces.pop();
		find_subfaces(current_hyperface, subfaces_of_current_hyperface);

		// Find intersection of subfaces_of_current_hyperface and bounding_subfaces.
		subfaces.m_data.clear();
		for (auto subface : subfaces_of_current_hyperface.m_data)
			if (bounding_subfaces.m_data.count(subface))
				subfaces.m_data.insert(subface);

		// Find n.
		cross_product_vectors.clear();

		for (auto it = std::next(current_hyperface.m_data.begin(), 1); it != current_hyperface.m_data.end(); ++it)
			cross_product_vectors.push_back(scatter_points[*it] - scatter_points[*current_hyperface.m_data.begin()]);

		normal_of_hyperface = MathVector::crossProduct(cross_product_vectors);
		normal_of_hyperface.normalize();

		// ����������� ������� ���������� � ���������������� ��� aff(F)
		// ��� �� �������� ��� ������������ ������ (�����������������), �.�. ��� ��. ��-�� ����� ���� == 0
		for (size_t counter = 0; counter < scatter_points.size(); ++counter)
			if (!current_hyperface.m_data.count(counter))
			{
				if ((scatter_points[counter] - scatter_points[*current_hyperface.m_data.begin()]) * normal_of_hyperface < 0)
					normal_of_hyperface = -1.0 * normal_of_hyperface;
				break;
			}

		for (auto subface : subfaces.m_data)
		{
			// Find a.
			cross_product_vectors.clear();
			cross_product_vectors.push_back(normal_of_hyperface);
			for (auto it = std::next(subface.m_data.begin(), 1); it != subface.m_data.end(); ++it)
				cross_product_vectors.push_back(scatter_points[*it] - scatter_points[*subface.m_data.begin()]);

			normal_of_subface = MathVector::crossProduct(cross_product_vectors);
			normal_of_subface.normalize();

			// (�������� ����� ���-�� ������ ������ ���������� ������� a)
			// ����������� ������� �������� � ��������������� ����������� �� ������� ������ ���������� ��� ������� ��������
			for (auto index : current_hyperface.m_data)
				if (!subface.m_data.count(index))
				{
					if ((scatter_points[index] - scatter_points[*subface.m_data.begin()]) * normal_of_subface > 0)
						normal_of_subface = -1.0 * normal_of_subface;
					break;
				}

			// Find vertex of a new hyperface.
			interest_indexes_of_points.clear();
			for (size_t counter = 0; counter < scatter_points.size(); ++counter)
				interest_indexes_of_points.insert(counter);

			for (auto vertex : current_hyperface.m_data)
				interest_indexes_of_points.erase(vertex);


			new_vertex_index = wrapping(false, normal_of_hyperface, scatter_points[*subface.m_data.begin()], normal_of_subface, interest_indexes_of_points);

			if (b_NonSimplicity)
				return;

			// Add new hyperface in queue.
			new_hyperface = subface;
			new_hyperface.m_data.insert(new_vertex_index);
			queue_of_hyperfaces.push(new_hyperface);

			// Recalculate set of bounding subfaces

			// 1) intersection_of_subfaces = � � subfaces(F')
			// � = bounding_subfaces; subfaces(F') = subfaces_of_new_hyperface

			find_subfaces(new_hyperface, subfaces_of_new_hyperface);

			intersection_of_subfaces.m_data.clear();
			for (auto subface_of_new_hyperface : subfaces_of_new_hyperface.m_data)
				if (bounding_subfaces.m_data.count(subface_of_new_hyperface))
					intersection_of_subfaces.m_data.insert(subface_of_new_hyperface.m_data);

			// 2) � = � U F'
			for (auto subface_of_new_hyperface : subfaces_of_new_hyperface.m_data)
				bounding_subfaces.m_data.insert(subface_of_new_hyperface);

			// 3) � = � \ intersection_of_subfaces
			for (auto subface_from_intersection : intersection_of_subfaces.m_data)
				bounding_subfaces.m_data.erase(subface_from_intersection);
		}

		convex_hull.m_data.insert(current_hyperface);
	}

	b_FinishAlgorithm = true;
}

void GiftWrapping::find_subfaces(const Face& hyperface, Faces& subfaces)
{
	subfaces.m_data.clear();

	// -----
	// std::unordered_set<size_t> subface;
	// std::set<size_t> subface;
	Face subface;
	// -----

	for (size_t indexSkip = 0; indexSkip < hyperface.m_data.size(); ++indexSkip)
	{
		subface.m_data.clear();

		auto it = hyperface.m_data.begin();

		for (size_t index = 0; index < hyperface.m_data.size(); ++index)
		{
			if (index != indexSkip)
				subface.m_data.insert(*it);

			++it;
		}

		subfaces.m_data.insert(subface);
	}
}

void GiftWrapping::scatter_operation()
{
	double max_coord_offset = 1e-3;
	size_t dimension = scatter_points[0].getDimension();

	boost::mt19937 generator;
	generator.seed(static_cast<unsigned>(std::time(0)));
	auto distribution = boost::uniform_real<>(0.0, max_coord_offset);

	std::vector<double> offset(dimension);

	for (auto it = scatter_points.begin(); it != scatter_points.end(); ++it)
	{
		for (size_t i = 0; i < dimension; ++i)
			offset[i] = distribution(generator);

		*it = *it + offset;
	}
}

// all_points ���������� � ��������������� ������, ������ std::vector<MathVector>& all_points (��� const)
size_t GiftWrapping::wrapping(bool is_first_hyperface, const MathVector& normal_of_hyperface, const MathVector& point_of_subface, const MathVector& normal_of_subface, const std::unordered_set<size_t>& indexes_of_candidates)
{
	double eps = 1e-10;

	MathVector vector_in_new_candidate_hyperface = scatter_points[*indexes_of_candidates.begin()] - point_of_subface;

	double dot_v_a = vector_in_new_candidate_hyperface * normal_of_subface;
	double abs_dot_v_n = fabs(vector_in_new_candidate_hyperface * normal_of_hyperface);

	if (abs_dot_v_n < eps)
	{
		if (!is_first_hyperface)
		{
			std::cout << "A non-simplicial case is found" << std::endl;
			b_NonSimplicity = true;
			scatter_operation();
			return 0;
		}

		// -----
		std::cout << "A non-simplicial case is found on 1st hyperface (status: alpha)" << std::endl;
		system("pause");
		exit(-1);

		/*
		// !!!!!
		if (fabs(dot_v_a) > eps)
		// !!!!!
			return *indexes_of_candidates.begin();
		//else
		//	std::cout << "wrapping warning: \"ctg\" should be = +inf" << std::endl;
		*/
		// -----
	}

	double ctg = -dot_v_a / abs_dot_v_n;
	double min_ctg = ctg;
	size_t index_of_point_with_min_ctg = *indexes_of_candidates.begin();

	for (auto it = std::next(indexes_of_candidates.begin(), 1); it != indexes_of_candidates.end(); ++it)
	{
		vector_in_new_candidate_hyperface = scatter_points[*it] - point_of_subface;

		dot_v_a = vector_in_new_candidate_hyperface * normal_of_subface;

		abs_dot_v_n = fabs(vector_in_new_candidate_hyperface * normal_of_hyperface);

		if (abs_dot_v_n < eps)
		{
			if (!is_first_hyperface)
			{
				std::cout << "A non-simplicial case is found" << std::endl;
				
				b_NonSimplicity = true;
				scatter_operation();
				return 0;
				
			}

			// -----
			std::cout << "A non-simplicial case is found on 1st hyperface (status: alpha)" << std::endl;
			system("pause");
			exit(-1);

			/*
			// !!!!!
			if (fabs(dot_v_a) > eps)
			// !!!!!
				return *it;
			else
				continue;
			*/
			// -----
		}

		ctg = -dot_v_a / abs_dot_v_n;

		if (ctg < min_ctg)
		{
			min_ctg = ctg;
			index_of_point_with_min_ctg = *it;
		}

	}

	return index_of_point_with_min_ctg;
}

void GiftWrapping::create_coordinate_axis(size_t num_of_coordinate, size_t dimension, MathVector& coordinate_axis)
{
	coordinate_axis = std::vector<double>(dimension, 0.0);
	coordinate_axis[num_of_coordinate] = 1.0;
}

// -----
void GramSchmidtOrthogonalization(const std::vector<MathVector>& some_basis, std::vector<MathVector>& orthonormal_basis)
{
	orthonormal_basis.clear();

	MathVector v = some_basis[0];
	v.normalize();

	orthonormal_basis.push_back(v);

	for (size_t i = 1; i < some_basis.size(); ++i)
	{
		v = some_basis[i];

		for (size_t j = 0; j < i; ++j)
			v = v - (some_basis[i] * orthonormal_basis[j]) * orthonormal_basis[j];

		v.normalize();
		orthonormal_basis.push_back(v);
	}

}
// -----

void GiftWrapping::find_first_hyperface(Face& first_hyperface)
{
	// Initialize remaining indexes, which can be considered.
	std::unordered_set<size_t> indexes_of_candidates;
	for (size_t counter = 0; counter < scatter_points.size(); ++counter)
		indexes_of_candidates.insert(counter);

	// Find point with min first coordinate. First normal is (1, 0, ..., 0).
	double min_first_coordinate = scatter_points[0][0];
	size_t index_of_point_with_min_first_coordinate = 0;

	for (size_t counter = 1; counter < scatter_points.size(); ++counter)
		if (scatter_points[counter][0] < min_first_coordinate)
		{
			min_first_coordinate = scatter_points[counter][0];
			index_of_point_with_min_first_coordinate = counter;
		}

	first_hyperface.m_data.insert(index_of_point_with_min_first_coordinate);
	indexes_of_candidates.erase(index_of_point_with_min_first_coordinate);


	MathVector normal_of_subface;
	MathVector normal_of_hyperface;

	size_t dimension = scatter_points[0].getDimension();
	create_coordinate_axis(0, dimension, normal_of_hyperface);


	std::vector<MathVector> cross_product_vectors;
	MathVector coordinate_axis;
	size_t new_index;

	for (size_t counter_1 = 1; counter_1 < dimension; ++counter_1)
	{
		// Recalculate normal of a subface
		cross_product_vectors.clear();
		cross_product_vectors.push_back(normal_of_hyperface);

		auto it = std::next(first_hyperface.m_data.begin());

		for (size_t counter_2 = 1; counter_2 < counter_1; ++counter_2)
		{
			cross_product_vectors.push_back(scatter_points[*it] - scatter_points[*first_hyperface.m_data.begin()]);
			++it;
		}

		for (size_t counter_2 = counter_1 + 1; counter_2 < dimension; ++counter_2)
		{
			create_coordinate_axis(counter_2, dimension, coordinate_axis);
			cross_product_vectors.push_back(coordinate_axis);
		}

		normal_of_subface = MathVector::crossProduct(cross_product_vectors);
		normal_of_subface.normalize();

		new_index = wrapping(true, normal_of_hyperface, scatter_points[*first_hyperface.m_data.begin()], normal_of_subface, indexes_of_candidates);
		first_hyperface.m_data.insert(new_index);
		indexes_of_candidates.erase(new_index);

		// Recalculate normal of a hyperface
		cross_product_vectors.clear();

		it = std::next(first_hyperface.m_data.begin(), 1);
		for (size_t counter_2 = 1; counter_2 < counter_1 + 1; ++counter_2)
		{
			cross_product_vectors.push_back(scatter_points[*it] - scatter_points[*first_hyperface.m_data.begin()]);
			++it;
		}

		for (size_t counter_2 = counter_1 + 1; counter_2 < dimension; ++counter_2)
		{
			create_coordinate_axis(counter_2, dimension, coordinate_axis);
			cross_product_vectors.push_back(coordinate_axis);
		}

		MathVector old_normal_of_hyperface = normal_of_hyperface;

		normal_of_hyperface = MathVector::crossProduct(cross_product_vectors);

		double eps = 1e-5;
		if (normal_of_hyperface * normal_of_hyperface < eps)
			normal_of_hyperface = old_normal_of_hyperface;
		else
			normal_of_hyperface.normalize();
	}

	// -----
	if (input_points.size() == input_points[0].getDimension())
	{
		// ����� ����� �������� �������� input_points
		std::vector<MathVector> some_basis(input_points.size() - 1);
		
		for (size_t counter = 1; counter < input_points.size(); ++counter)
			some_basis[counter - 1] = input_points[counter] - input_points[0];

		std::vector<MathVector> orthonormal_basis(some_basis.size());

		GramSchmidtOrthogonalization(some_basis, orthonormal_basis);

		for (auto v : orthonormal_basis)
			std::cout << v << std::endl;

		system("pause");
		exit(-1);
	}
	// -----
}

