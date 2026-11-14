#include "registry/ClientRegistry.h"

#include <algorithm>

#include "error/BusinessRuleException.h"
#include "error/ClientNotFoundException.h"
#include "error/NotFoundException.h"
#include "error/ValidationException.h"

namespace repair
{

ClientRegistry::ClientRegistry() {}

std::size_t ClientRegistry::size() const { return clients_.size(); }

void ClientRegistry::registerClient(const std::shared_ptr<Client>& client)
{
    if (!client)
    {
        throw ValidationException("Регистрируемый клиент не задан");
    }
    if (contains(client->phone()))
    {
        throw BusinessRuleException("Клиент с телефоном " + client->phone().formatted()
                                    + " уже зарегистрирован");
    }
    clients_.push_back(client);
}

bool ClientRegistry::contains(const PhoneNumber& phone) const
{
    return std::any_of(clients_.begin(), clients_.end(),
                       [&phone](const std::shared_ptr<Client>& client)
                       { return client->phone() == phone; });
}

std::shared_ptr<Client> ClientRegistry::findByPhone(const PhoneNumber& phone) const
{
    const auto position = std::find_if(clients_.begin(), clients_.end(),
                                       [&phone](const std::shared_ptr<Client>& client)
                                       { return client->phone() == phone; });
    if (position == clients_.end())
    {
        throw ClientNotFoundException(phone.formatted());
    }
    return *position;
}

std::shared_ptr<Client> ClientRegistry::clientAt(std::size_t index) const
{
    if (index >= clients_.size())
    {
        throw NotFoundException("Клиент с таким номером отсутствует в реестре");
    }
    return clients_[index];
}

std::vector<std::shared_ptr<Client>> ClientRegistry::regularClients(int requiredOrders) const
{
    std::vector<std::shared_ptr<Client>> regulars;
    for (const std::shared_ptr<Client>& client : clients_)
    {
        if (client->isRegular(requiredOrders))
        {
            regulars.push_back(client);
        }
    }
    return regulars;
}

}
