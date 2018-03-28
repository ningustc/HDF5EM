#pragma once
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <H5Cpp.h>
namespace IO
{
	//this class is used for the saving of raw two dimensional 
	class EMdata_IO
	{
	public:
		EMdata_IO(std::string file_path_i, std::string dataset_name_i)
		{
			file_path = file_path_i;
			dataset_name = dataset_name_i;
			data_rank = 2;
		}
		void create_file()
		{
			H5::H5File file(file_path, H5F_ACC_TRUNC);
			H5::Group group_STEM(file.createGroup("/STEM"));
			group_STEM.createGroup("ABF");
			group_STEM.createGroup("HAADF");
			file.createGroup("/TEM");
			file.close();
		}
		void write_STEM_data(int row_i, int col_i, const std::vector<float> &HAADF_data, const std::vector<float> &ABF_data)
		{
			row = row_i; col = col_i;
			//open the created file, this will meet problem when the file dose not exit.
			H5::H5File file(file_path, H5F_ACC_RDWR);
			//create the dataset for the storage of the HAADF STEM and ABF data
			H5::Group HAADF_STEM_group = file.openGroup("/STEM/HAADF");
			H5::Group ABF_STEM_group = file.openGroup("/STEM/ABF");
			write_data(HAADF_STEM_group, HAADF_data);
			write_data(ABF_STEM_group, ABF_data);
			write_int_attribute(HAADF_STEM_group, "ROW", row);
			write_int_attribute(HAADF_STEM_group, "COL", col);
			write_int_attribute(ABF_STEM_group, "ROW", row);
			write_int_attribute(ABF_STEM_group, "COL", col);
		}
		void write_TEM_data(int row_i, int col_i, const std::vector<float> &TEM_data)
		{
			row = row_i; col = col_i;
			H5::H5File file(file_path, H5F_ACC_RDWR);
			H5::Group TEM_group = file.openGroup("/TEM");
			write_data(TEM_group, TEM_data);
			//write the row and column property to the file.
			write_int_attribute(TEM_group, "ROW", row);
			write_int_attribute(TEM_group, "COL", col);
		}
		void read_TEM_data(std::vector<float> &TEM_data, int &row_o, int &col_o)
		{
			H5::H5File file(file_path, H5F_ACC_RDWR);
			H5::Group TEM_group = file.openGroup("/TEM");
			read_int_attribute(TEM_group, "ROW", row_o);
			read_int_attribute(TEM_group, "COL", col_o);
			TEM_data.resize(row_o*col_o);
			read_data(TEM_group, TEM_data, row_o, col_o);
		}
		void read_STEM_data(std::vector<float> &ABF_data, std::vector<float> &HAADF_data, int &row_o, int &col_o)
		{
			H5::H5File file(file_path, H5F_ACC_RDWR);
			H5::Group HAADF_STEM_group = file.openGroup("/STEM/HAADF");
			H5::Group ABF_STEM_group = file.openGroup("/STEM/ABF");
			read_int_attribute(HAADF_STEM_group, "ROW", row_o);
			read_int_attribute(HAADF_STEM_group, "COL", col_o);
			HAADF_data.resize(row_o*col_o); ABF_data.resize(row_o*col_o);
			read_data(HAADF_STEM_group, HAADF_data, row_o, col_o);
			read_data(ABF_STEM_group, ABF_data, row_o, col_o);
		}
		void write_STEM_attributes(const float &attribute_val, const std::string &attribute_name)
		{
			H5::H5File file(file_path, H5F_ACC_RDWR);
			H5::Group HAADF_STEM_group = file.openGroup("/STEM/HAADF");
			H5::Group ABF_STEM_group = file.openGroup("/STEM/ABF");
			write_float_attribute(HAADF_STEM_group, attribute_name, attribute_val);
			write_float_attribute(ABF_STEM_group, attribute_name, attribute_val);
		}
		void write_TEM_attributes(const float &attribute_val, const std::string &attribute_name)
		{
			H5::H5File file(file_path, H5F_ACC_RDWR);
			H5::Group TEM_group = file.openGroup("/TEM");
			write_float_attribute(TEM_group, attribute_name, attribute_val);
		}
		void read_STEM_attributes(float &attribute_val, const std::string &attribute_name)
		{
			H5::H5File file(file_path, H5F_ACC_RDWR);
			H5::Group HAADF_STEM_group = file.openGroup("/STEM/HAADF");
			read_float_attribute(HAADF_STEM_group, attribute_name, attribute_val);
		}
		void read_TEM_attributes(float &attribute_val, const std::string &attribute_name)
		{
			H5::H5File file(file_path, H5F_ACC_RDWR);
			H5::Group TEM_group = file.openGroup("/TEM");
			read_float_attribute(TEM_group, attribute_name, attribute_val);
		}
	private:
		void write_data(H5::Group &EM_group, const std::vector<float> &HAADF_data)
		{
			std::vector<hsize_t> dims;
			dims.push_back(row); dims.push_back(col);
			H5::DataSpace dataspace(data_rank, dims.data());
			H5::DataSet EM_dataset = EM_group.createDataSet(dataset_name, H5::PredType::NATIVE_FLOAT, dataspace);
			EM_dataset.write(HAADF_data.data(), H5::PredType::NATIVE_FLOAT);
		}
		void write_float_attribute(H5::Group &EM_group, const std::string &attribute_name, const  float &attribute_val)
		{
			H5::DataSet dataset = EM_group.openDataSet(dataset_name);
			hsize_t dims[1] = { 1 };
			H5::DataSpace attr_dataspace = H5::DataSpace(1, dims);
			H5::Attribute attribute = dataset.createAttribute(attribute_name, H5::PredType::NATIVE_FLOAT, attr_dataspace);
			attribute.write(H5::PredType::NATIVE_FLOAT, &attribute_val);
		}
		void write_int_attribute(H5::Group &EM_group, const std::string &attribute_name, const  int &attribute_val)
		{
			H5::DataSet dataset = EM_group.openDataSet(dataset_name);
			hsize_t dims[1] = { 1 };
			H5::DataSpace attr_dataspace = H5::DataSpace(1, dims);
			H5::Attribute attribute = dataset.createAttribute(attribute_name, H5::PredType::NATIVE_INT32, attr_dataspace);
			attribute.write(H5::PredType::NATIVE_INT32, &attribute_val);
		}
		void read_data(H5::Group &EM_group, std::vector<float> &EM_data, int row, int col)
		{
			H5::DataSet EM_dataset = EM_group.openDataSet(dataset_name);
			EM_dataset.read(EM_data.data(), H5::PredType::NATIVE_FLOAT);
		}
		void read_int_attribute(H5::Group &EM_group, const std::string &attribute_name, int &attribute_val)
		{
			H5::DataSet dataset = EM_group.openDataSet(dataset_name);
			if (dataset.attrExists(attribute_name))
			{
				H5::Attribute attribute = dataset.openAttribute(attribute_name);
				H5::DataType type = attribute.getDataType();
				attribute.read(type, &attribute_val);
			}
		}
		void read_float_attribute(H5::Group &EM_group, const std::string &attribute_name, float &attribute_val)
		{
			H5::DataSet dataset = EM_group.openDataSet(dataset_name);
			if (dataset.attrExists(attribute_name))
			{
				H5::Attribute attribute = dataset.openAttribute(attribute_name);
				H5::DataType type = attribute.getDataType();
				attribute.read(type, &attribute_val);
			}
		}
		int data_rank;
		int row, col;
		std::string file_path;
		std::string dataset_name;
	};
}