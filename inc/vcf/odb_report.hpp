/**
 * Copyright 2016 EMBL - European Bioinformatics Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VCF_VALIDATOR_ODB_REPORT_HPP
#define VCF_VALIDATOR_ODB_REPORT_HPP


#include <odb/database.hxx>
#include "report_writer.hpp"
#include "report_reader.hpp"

namespace ebi
{
  namespace vcf
  {
    class OdbReportRW : public ReportWriter, public ReportReader
    {
      public:

        OdbReportRW(const std::string &db_name);

        // ReportWriter implementation
        virtual void write_error(Error &error) override;
        virtual void write_warning(Error &error) override;

        // ReportReader mplementation
        virtual size_t count_warnings() override;
        virtual void for_each_warning(std::function<void(std::shared_ptr<Error>)> user_function) override;
        virtual size_t count_errors() override;
        virtual void for_each_error(std::function<void(std::shared_ptr<Error>)> user_function) override;

      private:
        std::string db_name;
        std::unique_ptr<odb::core::database> db;
    };
  }
}


#endif //VCF_VALIDATOR_ODB_REPORT_HPP
