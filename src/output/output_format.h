/****
DIAMOND protein aligner
Copyright (C) 2013-2021 Max Planck Society for the Advancement of Science e.V.
                        Benjamin Buchfink
                        Eberhard Karls Universitaet Tuebingen
						
Code developed by Benjamin Buchfink <benjamin.buchfink@tue.mpg.de>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
****/

#pragma once
#include "basic/match.h"
#include "dp/flags.h"
#include "def.h"
#include "util/sequence/sequence.h"
#include "data/block/block.h"
#include "util/escape_sequences.h"
#include "basic/config.h"
#include "data/sequence_file.h"

namespace Output {

struct Info {
	SeqInfo query;
	bool unaligned;
	SequenceFile* db;
	TextBuffer& out;
	Extension::Stats stats;
	Util::Seq::AccessionParsing acc_stats;
	int64_t db_seqs, db_letters;
};

}

struct OutputFormat
{
	OutputFormat(unsigned code, HspValues hsp_values = HspValues::TRANSCRIPT, Output::Flags flags = Output::Flags::NONE, char query_separator = '\0') :
		code(code),
		query_separator(query_separator),
		needs_taxon_id_lists(false),
		needs_taxon_nodes(false),
		needs_taxon_scientific_names(false),
		needs_taxon_ranks(false),
		needs_paired_end_info(false),
		hsp_values(hsp_values),
		flags(flags)
	{}
	virtual void print_query_intro(Output::Info& info) const
	{}
	virtual void print_query_epilog(Output::Info& info) const
	{}
	virtual void print_match(const HspContext& r, Output::Info& info)
	{}
	virtual void print_header(Consumer &f, int mode, const char *matrix, int gap_open, int gap_extend, double evalue, const char *first_query_name, unsigned first_query_len) const
	{ }
	virtual void print_footer(Consumer &f) const
	{ }
	virtual OutputFormat* clone() const = 0;
	virtual ~OutputFormat()
	{ }
	static void print_title(TextBuffer &buf, const char *id, bool full_titles, bool all_titles, const char *separator, const EscapeSequences *esc = 0,  bool json_array = false);
	operator unsigned() const
	{
		return code;
	}
	const unsigned code;
	const char query_separator;
	bool needs_taxon_id_lists, needs_taxon_nodes, needs_taxon_scientific_names, needs_taxon_ranks, needs_paired_end_info;
	HspValues hsp_values;
	Output::Flags flags;
	enum { daa, blast_tab, blast_xml, sam, blast_pairwise, null, taxon, paf, bin1, EDGE, json };
};

struct Null_format : public OutputFormat
{
	Null_format() :
		OutputFormat(null)
	{}
	virtual OutputFormat* clone() const
	{
		return new Null_format(*this);
	}
};

struct DAA_format : public OutputFormat
{
	DAA_format();
	virtual OutputFormat* clone() const
	{
		return new DAA_format(*this);
	}
};

struct OutputField {
	const std::string key, clust_key, description;
	const HspValues hsp_values;
	const Output::Flags flags;
};

enum class Header { NONE, SIMPLE, VERBOSE };

struct TabularFormat : public OutputFormat
{
	TabularFormat(bool json = false);
    static const std::vector<OutputField> field_def;
    virtual void print_header(Consumer &f, int mode, const char *matrix, int gap_open, int gap_extend, double evalue, const char *first_query_name, unsigned first_query_len) const override;
    virtual void print_footer(Consumer &f) const override;
    virtual void print_query_intro(Output::Info& info) const override;
    virtual void print_match(const HspContext& r, Output::Info& info) override;
    virtual ~TabularFormat()
    { }
    virtual OutputFormat* clone() const override
    {
        return new TabularFormat(*this);
    }
    static Header header_format(unsigned workflow);
    void output_header(Consumer& f, bool cluster) const;
    std::vector<int64_t> fields;
    bool is_json;
};


struct PAF_format : public OutputFormat
{
	PAF_format():
		OutputFormat(paf, HspValues::TRANSCRIPT, Output::Flags::NONE)
	{}
	virtual void print_query_intro(Output::Info& info) const override;
	virtual void print_match(const HspContext& r, Output::Info& info) override;
	virtual ~PAF_format()
	{ }
	virtual OutputFormat* clone() const override
	{
		return new PAF_format(*this);
	}
};

struct Sam_format : public OutputFormat
{
	Sam_format():
		OutputFormat(sam, HspValues::TRANSCRIPT, Output::Flags::NONE)
	{ }
	virtual void print_match(const HspContext& r, Output::Info& info) override;
	virtual void print_header(Consumer &f, int mode, const char *matrix, int gap_open, int gap_extend, double evalue, const char *first_query_name, unsigned first_query_len) const override;
	virtual void print_query_intro(Output::Info& info) const override;
	virtual ~Sam_format()
	{ }
	virtual OutputFormat* clone() const override
	{
		return new Sam_format(*this);
	}
};

struct XML_format : public OutputFormat
{
	XML_format():
		OutputFormat(blast_xml, HspValues::TRANSCRIPT, Output::Flags::FULL_TITLES)
	{
		config.salltitles = true;
	} 
	virtual void print_match(const HspContext& r, Output::Info& info) override;
	virtual void print_header(Consumer &f, int mode, const char *matrix, int gap_open, int gap_extend, double evalue, const char *first_query_name, unsigned first_query_len) const override;
	virtual void print_query_intro(Output::Info& info) const override;
	virtual void print_query_epilog(Output::Info& info) const override;
	virtual void print_footer(Consumer &f) const override;
	virtual ~XML_format()
	{ }
	virtual OutputFormat* clone() const override
	{
		return new XML_format(*this);
	}
};

struct Pairwise_format : public OutputFormat
{
	Pairwise_format() :
		OutputFormat(blast_pairwise, HspValues::TRANSCRIPT, Output::Flags::FULL_TITLES)
	{
		config.salltitles = true;
	}
	virtual void print_match(const HspContext& r, Output::Info& info) override;
	virtual void print_header(Consumer &f, int mode, const char *matrix, int gap_open, int gap_extend, double evalue, const char *first_query_name, unsigned first_query_len) const override;
	virtual void print_query_intro(Output::Info& info) const override;
	virtual void print_query_epilog(Output::Info& infos) const override;
	virtual void print_footer(Consumer &f) const override;
	virtual ~Pairwise_format()
	{ }
	virtual OutputFormat* clone() const override
	{
		return new Pairwise_format(*this);
	}
};

struct TaxonFormat : public OutputFormat
{
	TaxonFormat();
	virtual void print_match(const HspContext& r, Output::Info& info) override;
	virtual void print_query_epilog(Output::Info& info) const override;
	virtual ~TaxonFormat()
	{ }
	virtual OutputFormat* clone() const override
	{
		return new TaxonFormat(*this);
	}
	unsigned taxid;
	double evalue;
};

namespace Output { namespace Format {

struct Edge : public OutputFormat
{
	struct Data {
		OId query, target;
		float qcovhsp, scovhsp;
		double evalue;
	};
	Edge() :
		OutputFormat(EDGE, HspValues::COORDS)
	{}
	virtual void print_match(const HspContext& r, Output::Info& info) override;
	virtual ~Edge()
	{ }
	virtual OutputFormat* clone() const override
	{
		return new Edge(*this);
	}
};

}}

OutputFormat* get_output_format();
OutputFormat* init_output(int64_t& max_target_seqs);
void print_hsp(Hsp &hsp, const TranslatedSequence &query);
void print_cigar(const HspContext &r, TextBuffer &buf);