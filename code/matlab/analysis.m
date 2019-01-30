
data = loadData('../build-debug/tests/data.json');


% colors = hsv(length(data.lineages));
% for l=1:length(data.lineages)
%     lineage = data.lineages{l};
%     for cc = 1:length(lineage)
%         icc = lineage(cc);
%         plot(data.table.t(data.cell_cycles{icc}),data.table.q_0(data.cell_cycles{icc})./data.table.V(data.cell_cycles{icc}),'Color',colors(l,:)); hold on;
%     end
% end